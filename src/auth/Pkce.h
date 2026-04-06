// SPDX-License-Identifier: MIT
// Copyright (c) 2026 XiQiu
#pragma once

#include <QString>

namespace Pkce {
QString randomUrlSafeString(int byteCount);
QString generateVerifier();
QString challengeFromVerifier(const QString &verifier);
QString generateState();
}
