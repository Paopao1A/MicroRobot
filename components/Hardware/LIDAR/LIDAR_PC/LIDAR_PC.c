#include "LIDAR_PC.h"

void LIDAR_RECEIVE_BYTE(LIDAR_Base_t *self, uint8_t data)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LIDAR_RECEIVE_BYTE != NULL);
    self->ops->LIDAR_RECEIVE_BYTE(self, data);
}

bool LIDAR_HAS_NEW_SCAN(LIDAR_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LIDAR_HAS_NEW_SCAN != NULL);
    return self->ops->LIDAR_HAS_NEW_SCAN(self);
}

void LIDAR_CLEAR_NEW_SCAN(LIDAR_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LIDAR_CLEAR_NEW_SCAN != NULL);
    self->ops->LIDAR_CLEAR_NEW_SCAN(self);
}

void LIDAR_GET_SCAN(LIDAR_Base_t *self, LIDAR_Scan_t *scan)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->LIDAR_GET_SCAN != NULL);
    self->ops->LIDAR_GET_SCAN(self, scan);
}
