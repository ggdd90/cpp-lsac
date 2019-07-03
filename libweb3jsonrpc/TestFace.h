#ifndef JSONRPC_CPP_STUB_DEV_RPC_TESTFACE_H_
#define JSONRPC_CPP_STUB_DEV_RPC_TESTFACE_H_

#include "ModularServer.h"

namespace dev {
    namespace rpc {
        class TestFace : public ServerInterface<TestFace>
        {
            public:
                TestFace()
                {
                    this->bindAndAddMethod(jsonrpc::Procedure("test_getLogHash", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param1", jsonrpc::JSON_STRING, NULL), &dev::rpc::TestFace::test_getLogHashI);
                    this->bindAndAddMethod(jsonrpc::Procedure("test_importRawBlock", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, "param1", jsonrpc::JSON_STRING, NULL), &dev::rpc::TestFace::test_importRawBlockI);
                    this->bindAndAddMethod(jsonrpc::Procedure("test_setChainParams", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_BOOLEAN, "param1",jsonrpc::JSON_OBJECT, NULL), &dev::rpc::TestFace::test_setChainParamsI);
                    this->bindAndAddMethod(jsonrpc::Procedure("test_mineBlocks", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_BOOLEAN, "param1",jsonrpc::JSON_INTEGER, NULL), &dev::rpc::TestFace::test_mineBlocksI);
                    this->bindAndAddMethod(jsonrpc::Procedure("test_modifyTimestamp", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_BOOLEAN, "param1",jsonrpc::JSON_INTEGER, NULL), &dev::rpc::TestFace::test_modifyTimestampI);
                    this->bindAndAddMethod(jsonrpc::Procedure("test_rewindToBlock", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_BOOLEAN, "param1",jsonrpc::JSON_INTEGER, NULL), &dev::rpc::TestFace::test_rewindToBlockI);
                }
                inline virtual void test_getLogHashI(const Json::Value& request, Json::Value& response)
                {
                    response = this->test_getLogHash(request[0u].asString());
                }
                inline virtual void test_importRawBlockI(const Json::Value& request, Json::Value& response)
                {
                    response = this->test_importRawBlock(request[0u].asString());
                }
                inline virtual void test_setChainParamsI(const Json::Value &request, Json::Value &response)
                {
                    response = this->test_setChainParams(request[0u]);
                }
                inline virtual void test_mineBlocksI(const Json::Value &request, Json::Value &response)
                {
                    response = this->test_mineBlocks(request[0u].asInt());
                }
                inline virtual void test_modifyTimestampI(const Json::Value &request, Json::Value &response)
                {
                    response = this->test_modifyTimestamp(request[0u].asInt());
                }
                inline virtual void test_rewindToBlockI(const Json::Value &request, Json::Value &response)
                {
                    response = this->test_rewindToBlock(request[0u].asInt());
                }
                virtual std::string test_getLogHash(const std::string& param1) = 0;
                virtual std::string test_importRawBlock(const std::string& param1) = 0;
                virtual bool test_setChainParams(const Json::Value& param1) = 0;
                virtual bool test_mineBlocks(int param1) = 0;
                virtual bool test_modifyTimestamp(int param1) = 0;
                virtual bool test_rewindToBlock(int param1) = 0;
        };

    }
}
#endif //JSONRPC_CPP_STUB_DEV_RPC_TESTFACE_H_
