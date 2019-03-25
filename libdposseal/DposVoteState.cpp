#include "DposVoteState.h"
#include <libethcore/TransactionBase.h>

using namespace dev::eth;

/**********************DposState start****************************/
void dev::bacd::DposVoteState::excuteTransation(TransactionBase const & _t, BlockHeader const & _h)
{
    //根据data的 合约数据解析，判断是否为投票合约，以及初始化 DposTransaTionResult 数据
    DposContractCreation contranct;
    contranct.populate(_t.data());
    if(!contranct.isContractCreation())
        return;
    if(contranct.m_type >= (size_t)e_max)
        return;
    DposTransaTionResult t_ret;
    t_ret.m_hash = _t.sha3();
    t_ret.m_epoch = _h.timestamp() / m_config.epochInterval; //time(NULL) / epochInterval;
   /* t_ret.m_form = _t.from();
    t_ret.m_send_to = _t.to();
    t_ret.m_type = (EDposDataType)_t.value();*/
    t_ret.m_block_hight = _h.number();
    t_ret.m_data = _t.data();
    t_ret.m_form = contranct.m_from;
    t_ret.m_send_to = contranct.m_send_to;
    t_ret.m_type = (EDposDataType)contranct.m_type;

    m_transations.push_back(t_ret);
    m_onResult.push_back(OnDealTransationResult(EDPosResult::e_Add, false, t_ret));
    cdebug <<EthYellow "excute transation ret:" << t_ret << EthYellow;
}

void dev::bacd::DposVoteState::excuteTransation(bytes const & _t, BlockHeader const & _h)
{
    TransactionBase _tb = TransactionBase(_t, CheckTransaction::Cheap);
    excuteTransation(_tb, _h);
}

void dev::bacd::DposVoteState::verifyVoteTransation(BlockHeader const & _h, h256s const & _t_hashs)
{
    if(m_transations.empty() || _t_hashs.empty())
        return;
    /* for (auto val : _t_hashs)
     {
         LOG(m_logger) << EthYellow << _h.number() - m_config.verifyVoteNum << " Block transations hash" << val << EthYellow;
     }*/
    m_onResult.clear();
    size_t curr_epoch = _h.timestamp() / m_config.epochInterval;
    std::vector<DposTransaTionResult>::iterator iter = m_transations.begin();
    for(; iter != m_transations.end();)
    {
        //LOG(m_logger) << EthYellow "m_transations epoch:" << iter->m_epoch << "|cuur_epoch :" << curr_epoch << EthYellow;
        LOG(m_logger) << EthYellow "result:" << *iter << EthYellow;
        if(iter->m_epoch != curr_epoch)   // 必须实在同一轮出块时间
        {
            LOG(m_logger) << EthYellow "dell Transation hash:" << iter->m_hash << "| tranch epoch:" << iter->m_epoch << "|now:" << curr_epoch << EthYellow;
            //标记此票失效 接下来归还抵押代币
            iter->m_type = e_timeOut;
            m_onResult.push_back(OnDealTransationResult(EDPosResult::e_Dell, false, *iter));
            iter = m_transations.erase(iter);
        }
        else
        {
            //在第6个块之后执行
            if(iter->m_block_hight <= (int64_t)(_h.number() - m_config.verifyVoteNum))
            {
                //区块数达到要求   执行交易
                DposTransaTionResult tranRet = *iter;
                auto ret = std::find(_t_hashs.begin(), _t_hashs.end(),tranRet.m_hash);
                if(ret != _t_hashs.end())
                {
                    LOG(m_logger) << "will deal Transation hash:" << tranRet.m_hash;
                    iter = m_transations.erase(iter);
                    m_onResult.push_back(OnDealTransationResult(EDPosResult::e_Dell, true, tranRet));
                }
                else
                {
                    LOG(m_logger) << "cant't find Transation hash:" << iter->m_hash;
                    ++iter;
                }
            }
            else
            {
                LOG(m_logger) << "verify BlockNum not enough Transation hash:" << iter->m_hash;
                ++iter;
            }
        }
    }
}

void dev::bacd::DposVoteState::updateVoteTransation(OnDealTransationResult const & ret)
{
    switch(ret.m_ret_type)
    {
    case e_Add:
    {
        /*DposTransaTionResult result;
        result.m_hash = ret.m_hash;
        result.m_epoch = ret.m_epoch;
        result.m_block_hight = ret.m_block_hight;
        result.m_form = ret.m_form;
        result.m_send_to = ret.m_send_to;
        result.m_type = ret.m_type;*/
        DposTransaTionResult result(static_cast<DposTransaTionResult>(ret));
        m_transations.push_back(result);
        break;
    }
    case  e_Dell:
    {

        std::vector<DposTransaTionResult>::iterator iter = m_transations.begin();
        for(; iter != m_transations.end(); ++iter)
        {
            if(iter->m_hash == ret.m_hash)
            {
                m_transations.erase(iter);
                return;
            }
        }
        break;
    }
    default:
    break;
    }
}

/**********************DposState end****************************/