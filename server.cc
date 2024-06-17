#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"

using grpc::CallbackServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerUnaryReactor;
using grpc::Status;

// Logic and data behind the server's behavior.
class ServerImpl final : public Simple::CallbackService {
  ServerUnaryReactor* Call(CallbackServerContext* context,
                            const Request* request,
                            Reply* reply) override {
    
    #ifdef TRIGGER_DEADLINE
      std::this_thread::sleep_for(std::chrono::seconds(1));
    #endif

    std::string prefix("Hello ");
    reply->set_rep(prefix + request->req());
    
    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  ServerImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}
