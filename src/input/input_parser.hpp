#ifndef __INPUT_PARSER_H__
#define __INPUT_PARSER_H__

enum class ParsedInput {
  up,
  down,
  left,
  right,
  next_valid,
  confirm,
  back,
  next_turn,
  quit,
  COUNT,
  manual,
  number,
  other,
  dot,
  interrupt
};
ParsedInput parse_input(int ch);
ParsedInput input_handler();

#endif  // __INPUT_PARSER_H__
