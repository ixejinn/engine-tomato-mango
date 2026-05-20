#ifndef MANGO_NETWORKCONFIG_H
#define MANGO_NETWORKCONFIG_H

namespace tomato {
    /// @brief Maximum number of ticks retained for rollback.
    /// 롤백을 위해 과거 상태를 보존하는 최대 틱 수.
    /// @note Must be a power of 2, as this value is used as the RingArray size.
    ///
    /// @note 추후 수정 필요한 임시 값

    // TODO: 네트워크 지연 커버리지 기준으로 수정
    // 100틱 - 1.67초 (최소한의 롤백 보장)
    // 300틱 - 5초
    // 500틱 - 8.33초
    constexpr int ROLLBACK_WINDOW{128};

    static constexpr const char* SERVER_ADDRESS = "192.168.31.234";
    static constexpr uint16_t SERVER_PORT_NUM = 7777;
}

#endif //MANGO_NETWORKCONFIG_H