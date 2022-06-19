/**
 * Copyright (c) 2019 Paul-Louis Ageneau, Murat Dogan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "rtc/rtc.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using std::shared_ptr;
using namespace std::chrono_literals;

int main(int argc, char **argv) {
    std::cout << "This is from client" << std::endl;

    rtc::InitLogger(rtc::LogLevel::Info);

    rtc::Configuration client_config;

    auto pc = std::make_shared<rtc::PeerConnection>(client_config);

    pc->onLocalDescription([](rtc::Description desc) {
        std::cout << "Client Local Description" << std::endl;
        std::cout << std::string(desc) << std::endl;
    });

    pc->onLocalCandidate([](rtc::Candidate cand) {
        std::cout << "Local candidate informtaion" << std::endl;
        std::cout << std::string(cand) << std::endl;
    });

    pc->onStateChange([](rtc::PeerConnection::State state) {
        std::cout << "Gathering state " << state << std::endl;
    });

    shared_ptr<rtc::DataChannel> dc;
    pc->onDataChannel([&](shared_ptr<rtc::DataChannel> _dc)
                      {
		std::cout << "[Got a DataChannel with label: " << _dc->label() << "]" << std::endl;
		dc = _dc;

		dc->onClosed(
		    [&]() { std::cout << "[DataChannel closed: " << dc->label() << "]" << std::endl; });

		dc->onMessage([](auto data) {
			if (std::holds_alternative<std::string>(data)) {
				std::cout << "[Received message: " << std::get<std::string>(data) << "]"
				          << std::endl;
			}
		}); });


    bool exit = false;
    while (!exit)
    {
        std::cout
            << std::endl
            << "***************************************CLIENT*************************************"
               "*****"
            << std::endl
            << "* 0: Exit /"
            << " 1: Enter remote description /"
            << " 2: Enter remote candidate /"
            << " 3: Send message /"
            << " 4: Print Connection Info *" << std::endl
            << "[Command]: ";

        int command = -1;
        std::cin >> command;
        std::cin.ignore();

        switch (command)
        {
        case 0:
        {
            exit = true;
            break;
        }
        case 1:
        {
            // Parse Description
            std::cout << "[Description]: ";
            std::string sdp, line;
            while (getline(std::cin, line) && !line.empty())
            {
                sdp += line;
                sdp += "\r\n";
            }
            std::cout << sdp;
            pc->setRemoteDescription(sdp);
            break;
        }
        case 2:
        {
            // Parse Candidate
            std::cout << "[Candidate]: ";
            std::string candidate;
            getline(std::cin, candidate);
            pc->addRemoteCandidate(candidate);
            break;
        }
        case 3:
        {
            // Send Message
            if (!dc || !dc->isOpen())
            {
                std::cout << "** Channel is not Open ** ";
                break;
            }
            std::cout << "[Message]: ";
            std::string message;
            getline(std::cin, message);
            dc->send(message);
            break;
        }
        case 4:
        {
            // Connection Info
            if (!dc || !dc->isOpen())
            {
                std::cout << "** Channel is not Open ** ";
                break;
            }
            rtc::Candidate local, remote;
            std::optional<std::chrono::milliseconds> rtt = pc->rtt();
            if (pc->getSelectedCandidatePair(&local, &remote))
            {
                std::cout << "Local: " << local << std::endl;
                std::cout << "Remote: " << remote << std::endl;
                std::cout << "Bytes Sent:" << pc->bytesSent()
                          << " / Bytes Received:" << pc->bytesReceived() << " / Round-Trip Time:";
                if (rtt.has_value())
                    std::cout << rtt.value().count();
                else
                    std::cout << "null";
                std::cout << " ms";
            }
            else
            {
                std::cout << "Could not get Candidate Pair Info" << std::endl;
            }
            break;
        }
        default:
        {
            std::cout << "** Invalid Command ** " << std::endl;
            break;
        }
        }
    }

    if (dc)
        dc->close();

    if (pc)
        pc->close();
}
