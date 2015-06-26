//--------------------------- Supplementary Methods ----------------------------------------------------//
void updatePosition() {
  switch (State) {
    case FORWARD:
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, leftWheelPwm + pwmAdjust, rightWheelPwm + pwmAdjust);
      break;
    case REVERSE:
      miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, leftWheelPwm + pwmAdjust, rightWheelPwm + pwmAdjust);
      break;
    case STOP:
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);
      break;
    case LINE_FOLLOWER:
      followLine();
      break;
    case OBSTACLE_AVOIDANCE:
      obstacleAvoidance();
      break;
    default:
      break;
      //todo : Make any extra required states
  }

}


