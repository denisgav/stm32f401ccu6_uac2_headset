#include "volume_ctrl.h"

uint16_t vol_to_db_convert(bool channel_mute, uint16_t channel_volume){
  if(channel_mute)
    return 0;

    // todo interpolate
  channel_volume += CENTER_VOLUME_INDEX;
  if (channel_volume < 0) channel_volume = 0;
  if (channel_volume >= COUTNOF_DB_TO_VOL) channel_volume = COUTNOF_DB_TO_VOL - 1;
  uint16_t vol_mul = db_to_vol[((uint16_t)channel_volume)];

  return vol_mul;
}

uint16_t vol_to_db_convert_enc(bool channel_mute, uint16_t channel_volume){
  if(channel_mute)
      return 0;

    // todo interpolate
  channel_volume += CENTER_VOLUME_INDEX << ENC_NUM_OF_FP_BITS;
  if (channel_volume < 0) channel_volume = 0;
  if (channel_volume >= COUTNOF_DB_TO_VOL << ENC_NUM_OF_FP_BITS) channel_volume = (COUTNOF_DB_TO_VOL << ENC_NUM_OF_FP_BITS) - 1;
  uint16_t vol_mul = db_to_vol[((uint16_t)channel_volume) >> ENC_NUM_OF_FP_BITS];

  return vol_mul;
}
