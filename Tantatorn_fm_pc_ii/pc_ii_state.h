// Defined enum
enum pc2_fm_state {
  AWAITING_PC1,
  AWAITING_PC2,
  SENDING_PC1,
  LAST_STATE
};

pc2_fm_state state = AWAITING_PC1;