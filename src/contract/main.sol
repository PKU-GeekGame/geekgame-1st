pragma solidity ^0.5.13;

contract checker
{
    address owner;
    uint256 ownerhashed;
    uint256 key;
    uint256 cipher;
    constructor (uint256 hs) public
    {
        owner=msg.sender;
        ownerhashed=hs;
    }
    function set_var(uint256 new_key,uint256 new_cipher) public
    {
        uint256 s1=uint256(keccak256(abi.encode(msg.sender)));
        uint256 s2=uint256(keccak256(abi.encode(tx.origin)));
        require(owner==msg.sender || owner==tx.origin || ownerhashed==s1 || ownerhashed==s2,"caller must be owner");
        key=new_key;
        cipher=new_cipher;
    }
    
    function check_flag(uint256 flag) public view returns (bool)
    {
        uint256 ans=0;
        for(uint i=0;i<64;i++)
        {
            uint tp=((flag>>(4*i))+5*i+7*(key>>(4*i)))&0xf;
            ans=ans|(tp<<(4*i));
        }
        require(ans==cipher,"this is not the real flag!");
        return true;
    }
}


contract test
{
    address owner;
    uint256 ownerhashed;
    uint256 key;
    uint256 cipher;
    constructor () public
    {
        owner=msg.sender;
    }
    
    function hashvv(address vp) public view returns (uint256)
    {
        uint256 s1=uint256(keccak256(abi.encode(vp)));
        return s1;
    }
    
    function enc_flag(uint256 flag,uint256 key) public view returns (uint256)
    {
        uint256 ans=0;
        for(uint i=0;i<64;i++)
        {
            uint tp=((flag>>(4*i))+5*i+7*(key>>(4*i)))&0xf;
            ans=ans|(tp<<(4*i));
        }
        return ans;
    }
}

contract modc
{
    constructor (address toaddr,uint256 key,uint256 cip) public
    {
        checker(toaddr).set_var(key,cip);
        selfdestruct(msg.sender);
    }
}