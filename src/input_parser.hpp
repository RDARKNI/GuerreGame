#ifndef __INPUT_PARSER_H__
#define __INPUT_PARSER_H__

enum PARSED_INPUTS {
  UP,
  DOWN,
  RIGHT,
  LEFT,
  NEXT_VALID,
  CONF,
  BACK,
  NEXT_TURN,
  QUIT,
  COUNT,
  MANUAL,
  NUM,
  OTHER,
  DOT
};
PARSED_INPUTS parse_input(int ch);
PARSED_INPUTS input_handler(int current_player = 0);
#endif  // __INPUT_PARSER_H__
