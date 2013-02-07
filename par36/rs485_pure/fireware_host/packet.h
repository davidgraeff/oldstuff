#pragma once


struct uartPacket {
    uint8_t addrinfo;
    uint8_t cmd;
    uint8_t value;
};

inline uint8_t getDestAddr(struct uartPacket* p) {
	return (p->addrinfo & 0x0f);
}
inline uint8_t getSrcAddr(struct uartPacket* p) {return (p->addrinfo >> 4); }
inline void setDestAddr(struct uartPacket* p, uint8_t a) { p->addrinfo |= a & 0x0f;}
inline void setSrcAddr(struct uartPacket* p, uint8_t a) { p->addrinfo |= a<<4;}