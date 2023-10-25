#include <stdio.h>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>

const int8_t JSON_PTYPE_OBJ = 0;
const int8_t JSON_PTYPE_ARR = 1;

const int8_t JSON_KEY_TYPE_OBJ = 0;
const int8_t JSON_KEY_TYPE_ARR = 1;
const int8_t JSON_KEY_TYPE_NULL = 2;

const int8_t JSON_VAL_TYPE_NULL = -1;
const int8_t JSON_VAL_TYPE_STRING = 0;
const int8_t JSON_VAL_TYPE_DECIMAL = 1;
const int8_t JSON_VAL_TYPE_FLOAT = 2;
const int8_t JSON_VAL_TYPE_BOOL = 3;

const int8_t ACT_FIND_KEY = 1;
const int8_t ACT_READ_KEY = 2;
const int8_t ACT_FIND_COL = 3;
const int8_t ACT_FIND_ARR_VAL = 4;
const int8_t ACT_FIND_VAL = 5;
const int8_t ACT_READ_VAL = 6;

struct json_key_t
{
    int8_t type = JSON_KEY_TYPE_OBJ; 
    std::string key = "";
};

struct json_val_t
{
    int8_t type = JSON_KEY_TYPE_NULL; 
    std::string val = "";

    template <typename T>
    void get_value(T& destination) {
        if (std::is_same<T, bool>::value) 
        {
            destination = val == "true";
        } 
        else {
            std::istringstream ss(val);
            ss >> destination;
        }
    }
};

typedef std::function<void(std::string, json_val_t)> on_stream_data_cb_t;

class json_stream_parser
{
private:
    on_stream_data_cb_t _cb_data;

    // current action
    uint8_t _act = ACT_FIND_VAL; 
    // current parent type (object or array)
    uint8_t _ptype = JSON_PTYPE_OBJ; 
    // current key
    json_key_t _key; 
    // current value
    json_val_t _val; 
    // current depth
    int8_t _depth = -1; 

    void _notify_data();

    void _set_current_ptype(int8_t t);
    void _step_up_path();
    void _step_down_path();
    void _set_arr_key();
    void _increase_arr_key();
    void _clear_current_key();
    void _clear_current_val(int8_t t);
    void _append_current_key(char c);
    void _append_current_val(char c);
    void _sa(int8_t a);
    bool _find_value(char c);
    bool _read_value(char c);
public:
    json_stream_parser(on_stream_data_cb_t cb);
    void parse(char c);
};

