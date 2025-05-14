
# Prerequisites

[flow cli](https://developers.flow.com/tools/flow-cli/install)

all commands should be run from this folder (cadence folder)

# Scripts

## Get sequence number of the next executed event

command:
```bash
flow scripts execute GetNextSequenceNumber.cdc -n testnet
```

example result (uint64):
```bash
Result: 12
```

## Get current control value

command:
```bash
flow scripts execute GetControlValue.cdc -n testnet
```

example result (int64):
```bash
Result: -5
```

# Transactions

prerequisite: change the `flow.json` private key at `account.microcontroller.key`

## Set control value to X (int64)

note: I don't know how to use flow cli arguments to set the control value to a negative value!

command
```bash
flow transactions send ChangeControlValue.cdc X --signer microcontroller -n testnet
```

example result (int64):
```bash
Transaction ID: 7e6d5472cdc6c90fa3e5771a47af1e17915b134c5f12c995b0151b4332b85231

Block ID        e158c142037e57d7e025fef4b8a3283be37ec8af7f806d5ace931d672281d68c
Block Height    258532186
Status          ✅ SEALED
ID              7e6d5472cdc6c90fa3e5771a47af1e17915b134c5f12c995b0151b4332b85231
Payer           0d3c8d02b02ceb4c
Authorizers     [0d3c8d02b02ceb4c]

Proposal Key:
    Address     0d3c8d02b02ceb4c
    Index       0
    Sequence    1

No Payload Signatures

Envelope Signature 0: 0d3c8d02b02ceb4c
Signatures (minimized, use --include signatures)

Events:
    Index       0
    Type        A.0d3c8d02b02ceb4c.MicrocontrollerTest.ControlValueChanged
    Tx ID       7e6d5472cdc6c90fa3e5771a47af1e17915b134c5f12c995b0151b4332b85231
    Values
                - eventSequence (UInt64): 0
                - oldValue (Int64): 0
                - value (Int64): 12

    Index       1
    Type        A.7e60df042a9c0868.FlowToken.TokensWithdrawn
    Tx ID       7e6d5472cdc6c90fa3e5771a47af1e17915b134c5f12c995b0151b4332b85231
    Values
                - amount (UFix64): 0.00000424
                - from ((Address)?): 0x0d3c8d02b02ceb4c

    Index       2
    Type        A.9a0766d93b6608b7.FungibleToken.Withdrawn
    Tx ID       7e6d5472cdc6c90fa3e5771a47af1e17915b134c5f12c995b0151b4332b85231
    Values
                - amount (UFix64): 0.00000424
                - balanceAfter (UFix64): 100000.00099052
                - from ((Address)?): 0x0d3c8d02b02ceb4c
                - fromUUID (UInt64): 271579372189192
                - type (String): "A.7e60df042a9c0868.FlowToken.Vault"
                - withdrawnUUID (UInt64): 16492674547182
```