#include "usbd_storage_if.h"

#include <stdbool.h>
#include <string.h>

#define STORAGE_LUN_NBR 1U
#define STORAGE_BLK_SIZ 0x200U        /* 512 bytes  */
#define RAM_DISK_SIZE (64UL * 1024UL) /* 64 KB SRAM */
#define STORAGE_BLK_NBR (RAM_DISK_SIZE / STORAGE_BLK_SIZ)

static bool storage_ready = false;
static uint8_t ram_disk[RAM_DISK_SIZE];

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t STORAGE_Init_FS(uint8_t lun);
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num,
                                     uint16_t *block_size);
static int8_t STORAGE_IsReady_FS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr,
                              uint16_t blk_len);
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr,
                               uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS(void);

void MSC_FlashStorage_Init(
    void); /* kept for compatibility, now initializes RAM disk */
static bool Storage_CheckRange(uint32_t blk_addr, uint32_t blk_len);
static uint32_t Storage_BlockToAddress(uint32_t blk_addr);

/* 36-byte standard inquiry data */
static const int8_t STORAGE_Inquirydata_FS[] = {
    0x00, 0x80, 0x02, 0x02, (STANDARD_INQUIRY_DATA_LEN - 5),
    0x00, 0x00, 0x00, 'R',  'o',
    'c',  'k',  'e',  't',  'P',
    'i',  'R',  'A',  'M',  ' ',
    'D',  'i',  's',  'k',  ' ',
    ' ',  ' ',  ' ',  ' ',  ' ',
    ' ',  ' ',  '0',  '.',  '0',
    '1'};

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS = {
    STORAGE_Init_FS,      STORAGE_GetCapacity_FS,
    STORAGE_IsReady_FS,   STORAGE_IsWriteProtected_FS,
    STORAGE_Read_FS,      STORAGE_Write_FS,
    STORAGE_GetMaxLun_FS, (int8_t *)STORAGE_Inquirydata_FS};

/**
 * @brief  Initializes the storage unit (medium)
 * @param  lun: Logical unit number.
 * @retval USBD Status
 */
static int8_t STORAGE_Init_FS(uint8_t lun) {
  UNUSED(lun);
  MSC_FlashStorage_Init();
  return (storage_ready ? USBD_OK : USBD_FAIL);
}

/**
 * @brief  Returns the medium capacity.
 */
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num,
                                     uint16_t *block_size) {
  UNUSED(lun);
  *block_num = STORAGE_BLK_NBR;
  *block_size = STORAGE_BLK_SIZ;
  return USBD_OK;
}

/**
 * @brief  Checks whether the medium is ready.
 */
static int8_t STORAGE_IsReady_FS(uint8_t lun) {
  /* USER CODE BEGIN 4 */
  UNUSED(lun);
  return storage_ready ? USBD_OK : USBD_FAIL;
  /* USER CODE END 4 */
}

/**
 * @brief  Checks whether the medium is write protected.
 */
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun) {
  UNUSED(lun);
  return USBD_OK;
}

/**
 * @brief  Reads data from the medium.
 */
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr,
                              uint16_t blk_len) {
  UNUSED(lun);

  if (!storage_ready || !Storage_CheckRange(blk_addr, blk_len)) {
    return USBD_FAIL;
  }

  memcpy(buf, &ram_disk[Storage_BlockToAddress(blk_addr)],
         (uint32_t)blk_len * STORAGE_BLK_SIZ);
  return USBD_OK;
}

/**
 * @brief  Writes data into the medium.
 */
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr,
                               uint16_t blk_len) {
  UNUSED(lun);

  if (!storage_ready || !Storage_CheckRange(blk_addr, blk_len)) {
    return USBD_FAIL;
  }

  memcpy(&ram_disk[Storage_BlockToAddress(blk_addr)], buf,
         (uint32_t)blk_len * STORAGE_BLK_SIZ);
  return USBD_OK;
}

/**
 * @brief  Returns the Max Supported LUNs.
 */
static int8_t STORAGE_GetMaxLun_FS(void) { return STORAGE_LUN_NBR - 1; }

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
void MSC_FlashStorage_Init(void) {
  if (!storage_ready) {
    memset(ram_disk, 0xFF, sizeof(ram_disk));
    storage_ready = true;
  }
}

static bool Storage_CheckRange(uint32_t blk_addr, uint32_t blk_len) {
  return (blk_len > 0U) && ((blk_addr + blk_len) <= STORAGE_BLK_NBR);
}

static uint32_t Storage_BlockToAddress(uint32_t blk_addr) {
  return blk_addr * STORAGE_BLK_SIZ;
}