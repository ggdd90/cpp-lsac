#include "Address.h"

namespace dev
{
    Address const ZeroAddress;
    Address const MaxAddress        {"0xffffffffffffffffffffffffffffffffffffffff"};
    Address const SystemAddress     {"0xfffffffffffffffffffffffffffffffffffffffe"};
    Address const VoteAddress       { "0x00000000000000000000000000000000766f7465"};
    Address const systemAddress     { "0x000000000000000000000042616572436861696e"};

    //ExchangeFee 45786368616e6765466565
    Address const PdSystemAddress   { "0x00000000000000000045786368616e6765466565"};

    //7265637963656c
    Address const RecycelSystemAddress   { "0x000000000000000000000000007265637963656c"};
    //char [%d %x]

    Address const ExdbSystemAddress             { "0x0000000000000000006578646241646472657373"};

    Address const SysBlockCreateRecordAddress   { "0x000000537973425265636f726441646472657373"};
    Address const SysElectorAddress             { "0x000000000000456c6563746f7241646472657373"};
    Address const SysVarlitorAddress            { "0x000000000067656e657369735661726c69746f72"};
    Address const SysCanlitorAddress            { "0x0000000067656e6573697343616e646964617465"};
    Address const SysMinerSnapshotAddress       { "0x00000000005379734d696e657241646472657373"};


    // rdsn testAddress 
    Address const RdsnTestAddress               { "0x0000000000746573747264736e61646472657373"};

    std::vector<Address> const CreaterSysAddresses {
            Address("0x0000006365617465725379734164647265737331"),
            Address("0x0000006365617465725379734164647265737332"),
            Address("0x0000006365617465725379734164647265737333"),
            Address("0x0000006365617465725379734164647265737334"),
            Address("0x0000006365617465725379734164647265737335"),
            Address("0x0000006365617465725379734164647265737336"),
            Address("0x0000006365617465725379734164647265737337"),
            Address("0x0000006365617465725379734164647265737338"),
            Address("0x0000006365617465725379734164647265737339"),
            Address("0x0000636561746572537973416464726573733130"),
            Address("0x0000636561746572537973416464726573733131"),
            Address("0x0000636561746572537973416464726573733132"),
            Address("0x0000636561746572537973416464726573733133"),
            Address("0x0000636561746572537973416464726573733134"),
            Address("0x0000636561746572537973416464726573733135"),
            Address("0x0000636561746572537973416464726573733136"),
            Address("0x0000636561746572537973416464726573733137"),
            Address("0x0000636561746572537973416464726573733138"),
            Address("0x0000636561746572537973416464726573733139"),
            Address("0x0000636561746572537973416464726573733230"),
            Address("0x0000636561746572537973416464726573733231"),
            Address("0x0000636561746572537973416464726573733232"),
            Address("0x0000636561746572537973416464726573733233"),
            Address("0x0000636561746572537973416464726573733234"),
            Address("0x0000636561746572537973416464726573733235"),
            Address("0x0000636561746572537973416464726573733236"),
            Address("0x0000636561746572537973416464726573733237"),
            Address("0x0000636561746572537973416464726573733238"),
            Address("0x0000636561746572537973416464726573733239"),
            Address("0x0000636561746572537973416464726573733330"),
            Address("0x0000636561746572537973416464726573733331"),
            Address("0x0000636561746572537973416464726573733332"),
            Address("0x0000636561746572537973416464726573733333"),
            Address("0x0000636561746572537973416464726573733334"),
            Address("0x0000636561746572537973416464726573733335"),
            Address("0x0000636561746572537973416464726573733336"),
            Address("0x0000636561746572537973416464726573733337"),
            Address("0x0000636561746572537973416464726573733338"),
            Address("0x0000636561746572537973416464726573733339"),
            Address("0x0000636561746572537973416464726573733430"),
            Address("0x0000636561746572537973416464726573733431"),
            Address("0x0000636561746572537973416464726573733432"),
            Address("0x0000636561746572537973416464726573733433"),
            Address("0x0000636561746572537973416464726573733434"),
            Address("0x0000636561746572537973416464726573733435"),
            Address("0x0000636561746572537973416464726573733436"),
            Address("0x0000636561746572537973416464726573733437"),
            Address("0x0000636561746572537973416464726573733438"),
            Address("0x0000636561746572537973416464726573733439"),
            Address("0x0000636561746572537973416464726573733530"),
            Address("0x0000636561746572537973416464726573733531")
    };

}  // namespace dev

