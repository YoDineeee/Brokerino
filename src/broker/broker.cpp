#include "broker/broker.hpp"
#include "core/mqtt_codec.hpp"
#include "persistence/recovery.hpp"

#include <iostream>
#include <thread>

namespace brokerino {

Broker::Broker(const std::string& db_path, int port)
    : server_(port)
    , storage_(db_path) {

    auth_.AddUser("admin", "admin");
    auth_.AddToken("brokerino-default-token");

    std::vector<core::Session> recovered;
    persistence::Recover(storage_, recovered, sub_map_);

    for (auto& session : recovered)
        std::cout << "[Broker] Recovered session: " << session.client_id << "\n";
}

void Broker::Run() {
    std::cout << "[Broker] Starting on port " << server_.GetPort() << "\n";
    server_.Run([this](int fd) { OnClientConnected(fd); });
}

void Broker::Stop() {
    server_.Stop();
}

void Broker::OnClientConnected(int fd) {
    std::cout << "[Broker] New connection fd=" << fd << "\n";

    auto conn = std::make_unique<networking::Connection>(fd);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_[fd] = std::move(conn);
        buffers_[fd]     = {};
    }

    auto* conn_ptr = connections_[fd].get();

    std::thread([this, conn_ptr, fd]() {
        conn_ptr->Run(
            [this, fd](const std::vector<std::uint8_t>& data) {
                OnData(fd, data);
            },
            [this, fd]() {
                OnDisconnect(fd);
            }
        );
    }).detach();
}

void Broker::OnDisconnect(int fd) {
    std::lock_guard<std::mutex> lock(mutex_);

    const std::string client_id = ClientIdForFd(fd);

    if (!client_id.empty()) {
        auto it = sessions_.find(fd);
        if (it != sessions_.end() && it->second.clean_session) {
            storage_.DeleteSession(client_id);
            sub_map_.RemoveClient(client_id);
        }
    }

    connections_.erase(fd);
    sessions_.erase(fd);
    buffers_.erase(fd);

    std::cout << "[Broker] Client disconnected fd=" << fd << "\n";
}

void Broker::OnData(int fd, const std::vector<std::uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto& buf = buffers_[fd];
    buf.insert(buf.end(), data.begin(), data.end());

    while (true) {
        auto result = core::DecodeFixedHeader(buf);
        if (!result) break;

        const std::size_t total =
            result->header_size + result->header.remaining_length;

        if (buf.size() < total) break;

        const std::vector<std::uint8_t> payload(
            buf.begin() + static_cast<std::ptrdiff_t>(result->header_size),
            buf.begin() + static_cast<std::ptrdiff_t>(total)
        );

        switch (result->header.type) {
            case core::PacketType::CONNECT:
                HandleConnect(fd, payload);
                break;
            case core::PacketType::PUBLISH:
                HandlePublish(fd, result->header, payload);
                break;
            case core::PacketType::SUBSCRIBE:
                HandleSubscribe(fd, payload);
                break;
            case core::PacketType::PUBACK:
                HandlePuback(fd, payload);
                break;
            case core::PacketType::PINGREQ:
                HandlePingreq(fd);
                break;
            case core::PacketType::DISCONNECT:
                HandleDisconnect(fd);
                break;
            default:
                std::cout << "[Broker] Unknown packet on fd=" << fd << "\n";
                break;
        }

        buf.erase(buf.begin(),
                  buf.begin() + static_cast<std::ptrdiff_t>(total));
    }
}

void Broker::HandleConnect(int fd, const std::vector<std::uint8_t>& payload) {
    auto packet = core::ParseConnect(payload);
    if (!packet) {
        SendToClient(fd, core::SerializeConnack(
            {false, core::ConnectReturnCode::RefusedBadProtocol}));
        return;
    }

    if (!auth_.Authenticate(packet->username, packet->password, "")) {
        SendToClient(fd, core::SerializeConnack(
            {false, core::ConnectReturnCode::RefusedBadCredentials}));
        return;
    }

    auto session  = core::CreateSession(*packet);
    sessions_[fd] = session;

    if (!session.clean_session)
        storage_.SaveSession(session);

    for (const auto& [filter, qos] : session.subscriptions)
        sub_map_.Subscribe(session.client_id, filter, qos);

    SendToClient(fd, core::SerializeConnack(
        {false, core::ConnectReturnCode::Accepted}));

    std::cout << "[Broker] Accepted: " << packet->client_id << "\n";
}

void Broker::HandlePublish(int fd, const core::FixedHeader& header,
                            const std::vector<std::uint8_t>& payload) {
    auto packet = core::ParsePublish(header, payload);
    if (!packet) return;

    const auto subscribers = sub_map_.Match(packet->topic);

    for (const auto& client_id : subscribers) {
        for (auto& [cfd, session] : sessions_) {
            if (session.client_id != client_id) continue;

            if (packet->qos == 1) {
                packet->packet_id = core::NextPacketId(sessions_[cfd]);
                core::TrackInflight(sessions_[cfd], *packet);
                storage_.SaveMessage(client_id, *packet);
            }

            SendToClient(cfd, core::SerializePublish(*packet));
        }
    }

    if (packet->qos == 1 && packet->packet_id)
        SendToClient(fd, core::SerializePuback({*packet->packet_id}));
}

void Broker::HandleSubscribe(int fd, const std::vector<std::uint8_t>& payload) {
    auto packet = core::ParseSubscribe(payload);
    if (!packet) return;

    const std::string client_id = ClientIdForFd(fd);
    std::vector<std::uint8_t> return_codes;

    for (const auto& sub : packet->subscriptions) {
        sub_map_.Subscribe(client_id, sub.topic_filter, sub.qos);
        core::AddSubscription(sessions_[fd], sub.topic_filter, sub.qos);
        return_codes.push_back(sub.qos);
    }

    if (!sessions_[fd].clean_session)
        storage_.SaveSession(sessions_[fd]);

    SendToClient(fd, core::SerializeSuback({packet->packet_id, return_codes}));
}

void Broker::HandlePuback(int fd, const std::vector<std::uint8_t>& payload) {
    auto packet = core::ParsePuback(payload);
    if (!packet) return;

    const std::string client_id = ClientIdForFd(fd);
    core::AcknowledgeInflight(sessions_[fd], packet->packet_id);
    storage_.DeleteMessage(client_id, packet->packet_id);
}

void Broker::HandlePingreq(int fd) {
    SendToClient(fd, core::SerializePingresp());
}

void Broker::HandleDisconnect(int fd) {
    OnDisconnect(fd);
}

void Broker::SendToClient(int fd, const std::vector<std::uint8_t>& data) {
    auto it = connections_.find(fd);
    if (it == connections_.end()) return;
    it->second->Send(data);
}

std::string Broker::ClientIdForFd(int fd) const {
    auto it = sessions_.find(fd);
    if (it == sessions_.end()) return "";
    return it->second.client_id;
}

} // namespace brokerino
