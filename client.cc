#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

std::chrono::system_clock::time_point create_deadline(int timeout) {
    return std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
}

class SimpleClient {
    public:
        SimpleClient(std::shared_ptr<Channel> channel)
            : stub_(Simple::NewStub(channel)) {}

        // Assembles the client's payload, sends it and presents the response back
        // from the server.
        std::string call(const std::string& payload) {
            // Data we are sending to the server.
            Request request;
            request.set_req(payload);

            // The actual RPC
            (new Impl(stub_))->Call(request);
            
            return "finished";
        }

    private:
        class Impl {
            public:
                Impl(std::shared_ptr<Simple::Stub> stub_) : stub(stub_) {}

                void Call(Request req) {
                    ctx.set_deadline(create_deadline(100));
                    stub->async()->Call(&ctx, &req, &rep,
                                        [this](Status status) {
                                            if (status.ok()) {
                                                std::cout << "Client received: " << rep.rep() << std::endl;
                                            } else {
                                                std::cout << status.error_code() 
                                                        << ": " 
                                                        << status.error_message() 
                                                        << std::endl;
                                            }
                                            delete this;
                                        });
                }

            private:
                std::shared_ptr<Simple::Stub> stub;
                ClientContext ctx;
                Reply rep;
        };

    private:
        std::shared_ptr<Simple::Stub> stub_;
};

int main(int argc, char** argv) {
    SimpleClient simple(
        grpc::CreateChannel(std::string("localhost:50051"), grpc::InsecureChannelCredentials()));
    std::string reply = simple.call("world");
    std::cout << reply << std::endl;
    reply = simple.call("world");
    std::cout << reply << std::endl;
    reply = simple.call("world");
    std::cout << reply << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}
