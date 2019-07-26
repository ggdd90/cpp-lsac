#pragma once

#include <libdevcore/Common.h>
#include <libdevcore/SHA3.h>
#include <libdevcore/TrieCommon.h>
#include <libbrccore/Common.h>
#include "libbrccore/config.h"

#include <boost/filesystem/path.hpp>

namespace dev
{
class OverlayDB;

namespace brc
{
/**
 * Models the state of a single BrcdChain account.
 * Used to cache a portion of the full BrcdChain state. State keeps a mapping of Address's to
 * Accounts.
 *
 * Aside from storing the nonce and balance, the account may also be "dead" (where isAlive() returns
 * false). This allows State to explicitly store the notion of a deleted account in it's cache.
 * kill() can be used for this.
 *
 * For the account's storage, the class operates a cache. baseRoot() specifies the base state of the
 * storage given as the Trie root to be looked up in the state database. Alterations beyond this
 * base are specified in the overlay, stored in this class and retrieved with storageOverlay().
 * setStorage allows the overlay to be altered.
 *
 * The constructor allows you to create an one of a number of "types" of accounts. The default
 * constructor makes a dead account (this is ignored by State when writing out the Trie). Another
 * three allow a basic or contract account to be specified along with an initial balance. The fina
 * two allow either a basic or a contract account to be created with arbitrary values.
 */

/// vote data
struct PollData{
    Address     m_addr;
    u256        m_poll = 0;
    int64_t     m_time = 0;

    PollData(){ m_addr = Address(); }
    PollData(Address const& addr, u256 const& poll, int64_t time):m_addr (addr), m_poll(poll), m_time(time){}
    PollData(Address const& addr): m_addr(addr), m_poll(0), m_time(0){}

    PollData& operator = (PollData const& _p){
        m_addr = _p.m_addr;
        m_poll = _p.m_poll;
        m_time = _p.m_time;
        return *this;
    }

    void  streamRLP(RLPStream& _s) const{
        _s.appendList(3);
        _s<< m_addr << m_poll << (u256)m_time;
    }

    void populate(bytes const& _b){
        RLP _rlp(_b);
        m_addr = _rlp[0].convert<Address>(RLP::LaissezFaire);
        m_poll = _rlp[1].convert<u256>(RLP::LaissezFaire);
        m_time = (int64_t)_rlp[2].convert<u256>(RLP::LaissezFaire);
    }

    bool operator == (Address const& addr) const{
        return m_addr == addr;
    }

    bool  operator > (PollData const& p ) const{
        if( m_poll > p.m_poll)
            return true;
        if (m_time < p.m_time)
            return true;
        return false;
    }

};

struct VoteSnapshot{
    std::map< u256, std::map<Address, u256>> m_voteDataHistory; // vote to other data
    std::map< u256, u256> m_pollNumHistory;                     // get tickets by other
    std::map< u256, u256> m_blockSummaryHistory;                // create_block awards
    u256 numberofrounds = 0;                                    // the rounds of got awards
    u256 m_latest_round = 0;                                    // the last snapshot rounds of record
    VoteSnapshot(){}

    VoteSnapshot& operator = (VoteSnapshot const& s_v){
        clear();
        for(auto const& val : s_v.m_voteDataHistory){
            std::map<Address, u256> _temp = val.second;
            m_voteDataHistory[val.first] = _temp;
        }
        m_pollNumHistory.insert(s_v.m_pollNumHistory.begin(),s_v.m_pollNumHistory.end());
        m_blockSummaryHistory.insert(s_v.m_blockSummaryHistory.begin(),s_v.m_blockSummaryHistory.end());
        numberofrounds = s_v.numberofrounds;
        m_latest_round = s_v.m_latest_round;
        return  *this;
    }

    void  streamRLP(RLPStream& _s) const{
        _s.appendList(5);

        RLPStream vote_s(m_voteDataHistory.size());
        for(auto vote : m_voteDataHistory){
            RLPStream data_s(vote.second.size());
            for(auto const& v: vote.second){
                data_s.append<Address, u256>(std::make_pair(v.first, v.second));
            }
            vote_s.append<u256, bytes>(std::make_pair(vote.first, data_s.out()));
        }
        _s << vote_s.out();

        RLPStream poll_s;
        poll_s.appendList(m_pollNumHistory.size());
        for(auto const& poll : m_pollNumHistory){
            poll_s.append<u256, u256>(std::make_pair(poll.first, poll.second));
        }
        _s << poll_s.out();

        RLPStream block_s(m_blockSummaryHistory.size());
        for(auto const& b: m_blockSummaryHistory){
            block_s.append<u256, u256>(std::make_pair(b.first, b.second));
        }
        _s << block_s.out();
        _s << numberofrounds << m_latest_round;

    }
    void populate(bytes const& _b){
        RLP rlp(_b);
        bytes b_vote = rlp[0].toBytes();
        for(auto const& val: RLP(b_vote)){
            std::pair<u256 , bytes > _pair = val.toPair<u256, bytes>();
            std::map<Address, u256> vote_data;
            for(auto const& v: RLP(_pair.second)){
                std::pair<Address , u256> v_pair = v.toPair<Address, u256>();
                vote_data[v_pair.first] = v_pair.second;
            }
            m_voteDataHistory[_pair.first] = vote_data;
        }
        bytes  poll_b= rlp[1].toBytes();
        for(auto const& val : RLP(poll_b)){
            std::pair< u256, u256> _pair = val.toPair<u256,u256>();
            m_pollNumHistory[_pair.first] = _pair.second;
        }
        bytes block_b = rlp[2].toBytes();
        for(auto const& val: RLP(block_b)){
            std::pair< u256, u256> _pair = val.toPair<u256,u256>();
            m_blockSummaryHistory[_pair.first] = _pair.second;
        }
        numberofrounds = rlp[3].toInt<u256>();
        m_latest_round = rlp[4].toInt<u256>();
    }
    void clear(){
        m_voteDataHistory.clear();
        m_pollNumHistory.clear();
        m_blockSummaryHistory.clear();
        numberofrounds = 0;
        m_latest_round = 0;
    }

};

inline std::ostream& operator << (std::ostream& out, VoteSnapshot const& t){
    out <<std::endl;
    out<< "data_history:{";
    for(auto const& val : t.m_voteDataHistory){
        out<< "rounds:("<< val.first << ":";
        for(auto const& v : val.second){
            out<<"["<< v.first << ","<< v.second<<"]";
        }
        out<< ")";
    }
    out<< "}"<<std::endl;

    out << "poll:{";
    for(auto const& val: t.m_pollNumHistory){
        out<<"["<<val.first <<","<< val.second<<"]";
    }
    out << "}"<<std::endl;

    out << "blockSummaryHistory:{";
    for(auto const& val : t.m_blockSummaryHistory){
        out<<"["<<val.first <<","<< val.second<<"]";
    }
    out << "}"<<std::endl;

    out << "rounds:"<< t.numberofrounds<<std::endl;
    out << "atest_round:"<< t.m_latest_round<<std::endl;

    return out;
}

/// for record own  creater_block log
/**
 * If the latest recorded event is less than three creater_node round robin cycles then
 * the standby node starts to block in its place
 * */
struct BlockRecord{
    std::map<Address, int64_t > m_last_time;

    bytes streamRLP() const{
        RLPStream s(1);

        RLPStream time_s(m_last_time.size());
        for(auto const& val: m_last_time){
            time_s.append<Address, u256>(std::make_pair(val.first, val.second));
        }
        s<< time_s.out();
        return  s.out();
    }
    void populate(bytes const& b){
        RLP rlp(b);

        bytes time_b = rlp[0].toBytes();
        for(auto const& val: RLP(time_b)){
            std::pair<Address , u256 > pair = val.toPair<Address, u256>();
            m_last_time[pair.first] = (int64_t)pair.second;
        }
    }
    void set_last_block(std::pair<Address , int64_t >const& value){
        m_last_time[value.first] = value.second;
    }
    bool is_empty() const{
        return  m_last_time.empty();
    }
};

class Account
{
public:
    /// Changedness of account to create.
    enum Changedness
    {
        /// Account starts as though it has been changed.
        Changed,
        /// Account starts as though it has not been changed.
        Unchanged
    };

    /// Construct a dead Account.
    Account() {}

    /// Construct an alive Account, with given endowment, for either a normal (non-contract) account
    /// or for a contract account in the conception phase, where the code is not yet known.
    Account(u256 _nonce, u256 _balance, Changedness _c = Changed)
      : m_isAlive(true), m_isUnchanged(_c == Unchanged), m_nonce(_nonce), m_balance(_balance)
    {}

    Account(u256 _ballot, Changedness _c = Changed)
      : m_isAlive(true), m_isUnchanged(_c == Unchanged), m_nonce(0), m_ballot(_ballot)
    {}

	Account(u256 _nonce, u256 _balance, u256 _BRC, u256 _fbalance = 0, Changedness _c = Changed)
      : m_isAlive(true),
        m_isUnchanged(_c == Unchanged),
        m_nonce(_nonce),
        m_balance(_balance),
        m_BRC(_BRC),
		m_FBalance(_fbalance)
    {}

    /// Explicit constructor for wierd cases of construction or a contract account.
    Account(u256 _nonce, u256 _balance, h256 _contractRoot, h256 _codeHash, u256 _ballot,
        u256 _poll, u256 _BRC, u256 _FBRC, u256 _FBalance, Changedness _c, u256 _assetInjectStatus = 0)
      : m_isAlive(true),
        m_isUnchanged(_c == Unchanged),
        m_nonce(_nonce),
        m_balance(_balance),
        m_storageRoot(_contractRoot),
		m_codeHash(_codeHash), 
        m_ballot(_ballot), 
        m_poll(_poll),
        m_BRC(_BRC),
        m_FBRC(_FBRC),
        m_FBalance(_FBalance),
		m_assetInjectStatus(_assetInjectStatus)
    {
        assert(_contractRoot);
    }

 Account(u256 _nonce, u256 _balance, h256 _contractRoot, h256 _codeHash, Changedness _c)
		:
		m_isAlive(true),
        m_isUnchanged(_c == Unchanged),
        m_nonce(_nonce),
        m_balance(_balance),
        m_storageRoot(_contractRoot),
        m_codeHash(_codeHash)
    {
        assert(_contractRoot);
    }


    /// Kill this account. Useful for the suicide opcode. Following this call, isAlive() returns
    /// false.
    void kill()
    {
        m_isAlive = false;
        m_storageOverlay.clear();
        m_storageOriginal.clear();
        m_codeHash = EmptySHA3;
        m_storageRoot = EmptyTrie;
        m_balance = 0;
        m_BRC = 0;
        m_FBRC = 0;
        m_FBalance = 0;
        m_nonce = 0;
        m_poll = 0;
        m_ballot = 0;
		m_assetInjectStatus = 0;
        m_vote_data.clear();
        m_willChangeList.clear();
		m_BlockReward.clear();
        changed();
    }

    /// @returns true iff this object represents an account in the state. Returns false if this
    /// object represents an account that should no longer exist in the trie (an account that never
    /// existed or was suicided).
    bool isAlive() const { return m_isAlive; }

    /// @returns true if the account is unchanged from creation.
    bool isDirty() const { return !m_isUnchanged; }

    void untouch() { m_isUnchanged = true; }

    /// @returns true if the nonce, balance and code is zero / empty. Code is considered empty
    /// during creation phase.
    bool isEmpty() const {
        return nonce() == 0 && balance() == 0 && codeHash() == EmptySHA3 && BRC() == 0 &&
                FBalance() == 0 && FBRC() == 0  && CookieIncome() == 0 && m_vote_data.empty() &&
                m_BlockReward.size() == 0 && ballot() == 0 && m_block_records.is_empty() ;
    }

    /// @returns the balance of this account.
    u256 const& balance() const { return m_balance; }


    u256 const& BRC() const { return m_BRC; }

    u256 const& FBRC() const { return m_FBRC; }

    u256 const& FBalance() const { return m_FBalance; }
    std::vector<std::string> willChangeList() const { return m_willChangeList; }
    std::vector<std::string>& changeList() { return m_willChangeList; }

    /// Increments the balance of this account by the given amount.
    void addBalance(u256 _value)
    {
        m_balance += _value;
        changed();
    }

    // add BRC
    void addBRC(u256 _value)
    {
        m_BRC += _value;
        changed();
    }


	//add FBRC
    void addFBRC(u256 _value)
	{
		m_FBRC += _value;
        changed();
	}

    // add FCookie
    void addFBalance(u256 _value)
	{
		m_FBalance += _value;
        changed();
	}


    // Acounts own ballot
    u256 const& ballot() const { return m_ballot; }
    void addBallot(u256 _value) { m_ballot += _value; changed(); }

    // Acounts have poll by other
    u256 const& poll() const { return m_poll; }
    void addPoll(u256 _value) { m_poll += _value; changed(); }

    /// @returns the nonce of the account.
    u256 nonce() const { return m_nonce; }

	u256 assetInjectStatus() const{ return m_assetInjectStatus; }
	void setAssetInjectStatus() { m_assetInjectStatus = 1; }
    /// Increment the nonce of the account by one.
    void incNonce()
    {
        ++m_nonce;
        changed();
    }

    /// Set nonce to a new value. This is used when reverting changes made to
    /// the account.
    void setNonce(u256 const& _nonce)
    {
        m_nonce = _nonce;
        changed();
    }

    /// @returns the root of the trie (whose nodes are stored in the state db externally to this
    /// class) which encodes the base-state of the account's storage (upon which the storage is
    /// overlaid).
    h256 baseRoot() const
    {
        assert(m_storageRoot);
        return m_storageRoot;
    }

    /// @returns account's storage value corresponding to the @_key
    /// taking into account overlayed modifications
    u256 storageValue(u256 const& _key, OverlayDB const& _db) const
    {
        auto mit = m_storageOverlay.find(_key);
        if (mit != m_storageOverlay.end())
            return mit->second;

        return originalStorageValue(_key, _db);
    }

    /// @returns account's original storage value corresponding to the @_key
    /// not taking into account overlayed modifications
    u256 originalStorageValue(u256 const& _key, OverlayDB const& _db) const;

    /// @returns the storage overlay as a simple hash map.
    std::unordered_map<u256, u256> const& storageOverlay() const { return m_storageOverlay; }

    /// Set a key/value pair in the account's storage. This actually goes into the overlay, for
    /// committing to the trie later.
    void setStorage(u256 _p, u256 _v)
    {
        m_storageOverlay[_p] = _v;
        changed();
    }

    /// Empty the storage.  Used when a contract is overwritten.
    void clearStorage()
    {
        m_storageOverlay.clear();
        m_storageOriginal.clear();
        m_storageRoot = EmptyTrie;
        changed();
    }

    /// Set the storage root.  Used when clearStorage() is reverted.
    void setStorageRoot(h256 const& _root)
    {
        m_storageOverlay.clear();
        m_storageOriginal.clear();
        m_storageRoot = _root;
        changed();
    }

    /// @returns the hash of the account's code.
    h256 codeHash() const { return m_codeHash; }

    bool hasNewCode() const { return m_hasNewCode; }

    /// Sets the code of the account. Used by "create" messages.
    void setCode(bytes&& _code);

    /// Specify to the object what the actual code is for the account. @a _code must have a SHA3
    /// equal to codeHash().
    void noteCode(bytesConstRef _code)
    {
        assert(sha3(_code) == m_codeHash);
        m_codeCache = _code.toBytes();
    }

    /// @returns the account's code.
    bytes const& code() const { return m_codeCache; }

    bool insertMiner(Address before, Address after, unsigned blockNumber);
    bool changeMiner(unsigned blockNumber);
    bool changeVoteData(Address before, Address after);

    // VoteDate 投票数据
    u256 voteAll()const { u256 vote_num = 0; for(auto val : m_vote_data) vote_num += val.m_poll; return vote_num; }
    void set_vote_data(std::vector<PollData> const& _vote) { m_vote_data.clear(); m_vote_data.assign(_vote.begin(), _vote.end()); }

    /// this interface only for normalAddress
    void addVote(std::pair<Address, u256> _votePair);
    void manageSysVote(Address const& _otherAddr, bool _isLogin, u256 _tickets, int64_t _time =0);

    /// this interface only for systemAddress
    void set_system_poll(PollData const& _p);
    std::vector<PollData> const& vote_data() const { return  m_vote_data; }
    PollData poll_data(Address const& _addr) const;

    void sort_vote_data(){ std::sort(m_vote_data.begin(), m_vote_data.end(), std::greater<PollData>()); changed();}

	void addBlockRewardRecoding(std::pair<u256, u256> _pair);

    void initChangeList(std::vector<std::string> _changeList) {m_willChangeList = _changeList; }

	void setBlockReward(std::vector<std::pair<u256, u256>> const& _blockReward) { m_BlockReward.clear(); m_BlockReward = _blockReward;}
	std::vector<std::pair<u256, u256>> const& blockReward() const { return m_BlockReward; }

	std::unordered_map<Address, u256> findSnapshotSummary(uint32_t _snapshotNum);

    u256 findSnapshotSummaryForAddr(uint32_t _snapshotNum, Address _addr);

    u256 const& CookieIncome() const {return m_CooikeIncomeNum;}
    void setCookieIncome(u256 const& _value)
    {
        m_CooikeIncomeNum = _value;
        changed();
    }
    void addCooikeIncome(u256 _value)
    {
        m_CooikeIncomeNum += _value;
        changed();
    }

    /// Note that we've altered the account.
    void changed() { m_isUnchanged = false; }

    //interface about sanpshot
    void init_vote_snapshot(bytes const& _b){ m_vote_sapshot.populate(_b); }
    VoteSnapshot const& vote_snashot() const { return  m_vote_sapshot; }

    ///@return <true, rounds> if the snapshot need rocord new snapshot
    /// rounds: the last rounds need to snapshot
    std::pair<bool, u256> get_no_record_snapshot(u256 _rounds, Votingstage _state);

    /// update snapshot
    void try_new_snapshot(u256 _rounds);

    ///@retrue VoteSnapshot_data temp for verify
    VoteSnapshot try_new_temp_snapshot(u256 _rounds);

    void set_numberofrounds(u256 _val){
        m_vote_sapshot.numberofrounds = _val;
        changed();
    }
    void set_vote_snapshot(VoteSnapshot const& _vote_sna){
        m_vote_sapshot = _vote_sna;
        changed();
    }

    ///interface for Varlitor's create_block records
    void set_create_record(std::pair<Address , int64_t > const& value){
        m_block_records.set_last_block(value);
        changed();
    }
    int64_t last_records(Address const& _id) const{
        auto ret = m_block_records.m_last_time.find(_id);
        if(ret != m_block_records.m_last_time.end()){
            return ret->second;
        }
        return  0;
    }
    BlockRecord const& block_record() const { return  m_block_records;}
    void init_block_record(bytes const& _b){ m_block_records.populate(_b);}

private:
    /// Is this account existant? If not, it represents a deleted account.
    bool m_isAlive = false;

    /// True if we've not made any alteration to the account having been given it's properties
    /// directly.
    bool m_isUnchanged = false;

    /// True if new code was deployed to the account
    bool m_hasNewCode = false;

    /// Account's nonce.
    u256 m_nonce;


    /// Account's balance.
    u256 m_balance = 0;

    /// The base storage root. Used with the state DB to give a base to the storage.
    /// m_storageOverlay is overlaid on this and takes precedence for all values set.
    h256 m_storageRoot = EmptyTrie;

    /** If c_contractConceptionCodeHash then we're in the limbo where we're running the
     * initialisation code. We expect a setCode() at some point later. If EmptySHA3, then m_code,
     * which should be empty, is valid. If anything else, then m_code is valid iff it's not empty,
     * otherwise, State::ensureCached() needs to be called with the correct args.
     */
    h256 m_codeHash = EmptySHA3;

    // 自己拥有的票数 可以投给竞选节点的票数
    u256 m_ballot = 0;

    // 在投票开启中自己的得到的票数 不能使用
    u256 m_poll = 0;

	// Account's BRC
    u256 m_BRC = 0;

    // Account's FBRC
    u256 m_FBRC = 0;
    u256 m_FBalance = 0;

	u256 m_assetInjectStatus = 0;

	// Summary of the proceeds from the block address itself
	u256 m_CooikeIncomeNum = 0;

    std::vector<std::string> m_willChangeList;

    /// poll_data
    /// if this not is systemAddress : the address vote to other
    /// if this is systemAddress : this storage candidates_data or Varlitor ...
    std::vector<PollData> m_vote_data;

    // The snapshot about voteData
    VoteSnapshot    m_vote_sapshot;

    //std::unordered_map<u256, u256> m_BlockReward;

//	std::unordered_map <uint32_t, std::unordered_map<Address, u256>> m_cookieSummary;
//    std::map <uint32_t, bool> m_receiveStats;

    std::vector<std::pair<u256, u256>> m_BlockReward;
    /// The map with is overlaid onto whatever storage is implied by the m_storageRoot in the trie.
    mutable std::unordered_map<u256, u256> m_storageOverlay;

    /// The cache of unmodifed storage items
    mutable std::unordered_map<u256, u256> m_storageOriginal;

    /// The associated code for this account. The SHA3 of this should be equal to m_codeHash unless
    /// m_codeHash equals c_contractConceptionCodeHash.
    bytes m_codeCache;

    /// Value for m_codeHash when this account is having its code determined.
    static const h256 c_contractConceptionCodeHash;

    /// Varlitor's create_block records
    BlockRecord m_block_records;

};

class AccountMask
{
public:
    AccountMask(bool _all = false)
      : m_hasBalance(_all), m_hasNonce(_all), m_hasCode(_all), m_hasStorage(_all)
    {}

    AccountMask(bool _hasBalance, bool _hasNonce, bool _hasCode, bool _hasStorage,
        bool _shouldNotExist = false)
      : m_hasBalance(_hasBalance),
        m_hasNonce(_hasNonce),
        m_hasCode(_hasCode),
        m_hasStorage(_hasStorage),
        m_shouldNotExist(_shouldNotExist)
    {}

    bool allSet() const { return m_hasBalance && m_hasNonce && m_hasCode && m_hasStorage; }
    bool hasBalance() const { return m_hasBalance; }
    bool hasNonce() const { return m_hasNonce; }
    bool hasCode() const { return m_hasCode; }
    bool hasStorage() const { return m_hasStorage; }
    bool shouldExist() const { return !m_shouldNotExist; }

private:
    bool m_hasBalance;
    bool m_hasNonce;
    bool m_hasCode;
    bool m_hasStorage;
    bool m_shouldNotExist = false;
};

using AccountMap = std::unordered_map<Address, Account>;
using AccountMaskMap = std::unordered_map<Address, AccountMask>;

class PrecompiledContract;
using PrecompiledContractMap = std::unordered_map<Address, PrecompiledContract>;

AccountMap jsonToAccountMap(std::string const& _json, u256 const& _defaultNonce = 0,
    AccountMaskMap* o_mask = nullptr, PrecompiledContractMap* o_precompiled = nullptr,
    const boost::filesystem::path& _configPath = {});
}  // namespace brc
}  // namespace dev

