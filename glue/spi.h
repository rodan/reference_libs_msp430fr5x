#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

void spi_read_frame(const uint16_t baseAddress, uint8_t * pBuffer, const uint16_t size);

void spi_send_frame(const uint16_t baseAddress, uint8_t * pBuffer, const uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
