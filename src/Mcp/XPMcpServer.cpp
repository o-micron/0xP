/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#include <Mcp/XPMcpServer.h>

XPMcpServer::XPMcpServer(XPRegistry* const registry)
  : _registry(registry)
{
}

XPMcpServer::~XPMcpServer() {}

void
XPMcpServer::initialize()
{
    t = std::thread([]() {
        // /* Create MCP server */
        // oatpp::mcp::Server server;

        // /* Add prompts */
        // server.addPrompt(std::make_shared<prompts::CodeReview>());

        // /* Add resource */
        // server.addResource(std::make_shared<resource::File>());

        // /* Add tools */
        // server.addTool(std::make_shared<tools::Logger>());

        // /* Run server */
        // server.stdioListen();
    });
}

void
XPMcpServer::finalize()
{
    t.join();
}

void
XPMcpServer::update()
{
}
