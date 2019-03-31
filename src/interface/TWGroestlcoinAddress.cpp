// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Base58.h"
#include "../Groestlcoin/Address.h"

#include <TrezorCrypto/ecdsa.h>
#include <TrustWalletCore/TWGroestlcoinAddress.h>
#include <TrustWalletCore/TWPublicKey.h>

#include <cstring>

using namespace TW::Groestlcoin;

bool TWGroestlcoinAddressEqual(struct TWGroestlcoinAddress lhs, struct TWGroestlcoinAddress rhs) {
    return std::memcmp(lhs.bytes, rhs.bytes, Address::size) == 0;
}

bool TWGroestlcoinAddressIsValid(TWData *_Nonnull data) {
    return TWDataSize(data) == Address::size;
}

bool TWGroestlcoinAddressIsValidString(TWString *_Nonnull string) {
    auto& s = *reinterpret_cast<const std::string*>(string);
    return Address::isValid(s);
}

bool TWGroestlcoinAddressInitWithString(struct TWGroestlcoinAddress *_Nonnull address, TWString *_Nonnull string) {
    auto& s = *reinterpret_cast<const std::string*>(string);

    const auto decoded = TW::Base58::bitcoin.decodeCheck(s, TW::Hash::groestl512d);
    if (decoded.size() != Address::size) {
        return false;
    }

    std::copy(decoded.begin(), decoded.end(), address->bytes);
    return true;
}

bool TWGroestlcoinAddressInitWithData(struct TWGroestlcoinAddress *_Nonnull address, TWData *_Nonnull data) {
    if (TWDataSize(data) != Address::size) {
        return false;
    }
    TWDataCopyBytes(data, 0, Address::size, address->bytes);
    return true;
}

void TWGroestlcoinAddressInitWithPublicKey(struct TWGroestlcoinAddress *_Nonnull address, struct TWPublicKey *_Nonnull publicKey, uint8_t prefix) {
    address->bytes[0] = prefix;

    ecdsa_get_pubkeyhash(publicKey->impl.compressed().bytes.data(), HASHER_SHA2_RIPEMD,  address->bytes + 1);
}

TWString *_Nonnull TWGroestlcoinAddressDescription(struct TWGroestlcoinAddress address) {
    const auto str = TW::Base58::bitcoin.encodeCheck(address.bytes, address.bytes + Address::size, TW::Hash::groestl512d);
    return TWStringCreateWithUTF8Bytes(str.data());
}

TWData *_Nonnull TWGroestlcoinAddressData(struct TWGroestlcoinAddress address) {
    return TWDataCreateWithBytes(address.bytes, Address::size);
}
