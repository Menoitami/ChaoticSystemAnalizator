#pragma once

enum class MessageType : int {
    Unkniown = 0,
    SendSingleAttractorPoint,
    SendAllAttractorPoints,
    GetSystem,
    GetMethod
};
