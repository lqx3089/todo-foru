// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#include "Pkce.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QRandomGenerator>

namespace {
QString base64UrlNoPadding(const QByteArray &in)
{
    return QString::fromLatin1(in.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}
}

QString Pkce::randomUrlSafeString(int byteCount)
{
    QByteArray bytes;
    bytes.resize(byteCount);
    auto *rng = QRandomGenerator::system();
    for (int i = 0; i < byteCount; ++i) {
        bytes[i] = static_cast<char>(rng->bounded(256));
    }
    return base64UrlNoPadding(bytes);
}

QString Pkce::generateVerifier()
{
    // 32 bytes => ~43 chars in base64url, compliant with RFC 7636 length [43,128]
    return randomUrlSafeString(32);
}

QString Pkce::challengeFromVerifier(const QString &verifier)
{
    const QByteArray digest = QCryptographicHash::hash(verifier.toUtf8(), QCryptographicHash::Sha256);
    return base64UrlNoPadding(digest);
}

QString Pkce::generateState()
{
    return randomUrlSafeString(24);
}
