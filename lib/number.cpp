#include "number.h"


uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint239_t result;

    int index = 34 - (shift / 7) % 35;
    int shift_in_segment = shift % 7;
    value <<= shift_in_segment;
    
    while (value != 0){
        if (index < 0) {
            index = 34;
        }
        result.data[index] = value & 0b1111111;
        value >>= 7;
        --index;
    }
    for (int i = 34; i != 0; i--) {
        result.data[i] |= (shift << 7);
        shift = shift >> 1;
    }

    return result;
}

int get_length(const char* str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

int divide_by_two(char* decimal_str) {
    int carry = 0;
    int length = get_length(decimal_str);

    for (int i = 0; i < length; i++) {
        int current_digit = (decimal_str[i] - '0') + carry * 10;
        decimal_str[i] = (current_digit / 2) + '0';
        carry = current_digit % 2;
    }

    int first_non_zero = 0;
    while (decimal_str[first_non_zero] == '0' && decimal_str[first_non_zero + 1] != '\0') {
        first_non_zero++;
    }
    if (first_non_zero > 0) {
        for (int i = 0; i <= length - first_non_zero; i++) {
            decimal_str[i] = decimal_str[i + first_non_zero];
        }
    }

    return carry;
}

char* decimal_to_binary(const char* decimal_str) {
    int length = get_length(decimal_str);
    char* binary_result = (char*)malloc(length * 4 + 1); 
    char* temp_decimal = (char*)malloc(length + 1);

    for (int i = 0; i < length; i++) {
        temp_decimal[i] = decimal_str[i];
    }
    temp_decimal[length] = '\0';

    int index = 0;
    while (get_length(temp_decimal) > 1 || temp_decimal[0] != '0') {
        int remainder = divide_by_two(temp_decimal);
        binary_result[index++] = remainder + '0';
    }

    binary_result[index] = '\0';
    for (int i = 0; i < index / 2; i++) {
        char temp = binary_result[i];
        binary_result[i] = binary_result[index - i - 1];
        binary_result[index - i - 1] = temp;
    }

    free(temp_decimal);
    return binary_result;
}

uint239_t FromString(const char* str, uint64_t shift) {
    uint239_t result;
    result = FromInt(0, 0);
    for (int i = 0; str[i] != '\0'; ++i) {
        result = result * FromInt(10, 0);
        result = result + FromInt(str[i] - '0', 0);        
    }
    if (shift != 0) {
        result = FromShift(result, shift, 1);
    }
    return result;
}

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs) {
    uint64_t shift_lhs = GetShift(lhs);
    uint64_t shift_rhs = GetShift(rhs);

    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);

    uint239_t result;
    uint8_t count_difference = 0;

    for(int i = 34; i >= 0; i--){
        result.data[i] = reset_lhs.data[i] + reset_rhs.data[i] + count_difference;
        count_difference = result.data[i] >> 7;
        result.data[i] = (result.data[i] << 1) >> 1;
    }
    
    uint64_t summary_shift = shift_lhs + shift_rhs;
    uint239_t result_with_shift = FromShift(result, summary_shift, 1);

    return result_with_shift;

}

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs) {
    uint64_t shift_lhs = GetShift(lhs);
    uint64_t shift_rhs = GetShift(rhs);
    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);
    uint239_t result;

    int count_difference = 0;
    for (int i = 34; i >=0; i--){
        if (reset_lhs.data[i] - reset_rhs.data[i] < 0){
            result.data[i] = reset_lhs.data[i] - reset_rhs.data[i];
            count_difference = 1;
        } else {
            result.data[i] = reset_lhs.data[i] - reset_rhs.data[i] - count_difference;
            count_difference = 0;
        }
    }

    uint64_t summary_shift = shift_lhs - shift_rhs;
    uint239_t result_with_shift = FromShift(result, summary_shift, 1);
    return result_with_shift;
}

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs) {
    uint64_t shift_lhs = GetShift(lhs);
    uint64_t shift_rhs = GetShift(rhs);
    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);
    uint239_t result;

    while (reset_rhs != FromInt(0, 0)){
        result = result + reset_lhs;
        reset_rhs = reset_rhs - FromInt(1, 0);
    }

    uint239_t result_with_shift = FromShift(result, shift_lhs + shift_rhs, 1);
    return result_with_shift;
}

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs) {
    uint64_t shift_lhs = GetShift(lhs);
    uint64_t shift_rhs = GetShift(rhs);
    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);
    uint239_t result;

    uint239_t left_bottom = FromInt(1, 0);
    uint239_t right_bottom = reset_lhs;
    uint239_t mid;

    if (reset_rhs == FromInt(1, 0)){
        return FromShift(reset_lhs, shift_lhs - shift_rhs, 1);
    }

    while(right_bottom > left_bottom || right_bottom == left_bottom){
        mid = DivideByTwo(left_bottom + right_bottom);
        if ((mid * reset_rhs) > reset_lhs){
            right_bottom = mid - FromInt(1, 0);
        } else {
            if (mid * reset_rhs == reset_lhs){
                return FromShift(mid, shift_lhs - shift_rhs, 1);
            }
            result = mid;
            left_bottom = mid + FromInt(1, 0);
        }
    }

    uint64_t summary_shift = shift_lhs - shift_rhs;
    uint239_t result_with_shift = result;
    if (summary_shift != 0){
        result_with_shift = FromShift(result, summary_shift, 1);
    }
    return result_with_shift;


}

bool operator==(const uint239_t& lhs, const uint239_t& rhs) {
    int shift_lhs = GetShift(lhs);
    int shift_rhs = GetShift(rhs);

    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);

    for (int i = 34; i != 0; i--){
        uint8_t n1 = reset_lhs.data[i];
        uint8_t n2 = reset_rhs.data[i];
        if (n1 == n2 && n1 == 0){
            continue;
        }
        if (n1 != n2) {
            return false;
        }
    }
    return true;

}

bool operator!=(const uint239_t& lhs, const uint239_t& rhs) {
    int shift_lhs = GetShift(lhs);
    int shift_rhs = GetShift(rhs);
    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);

    if (reset_lhs == reset_rhs){
        return false;
    }
    return true;
}

bool operator<=(const uint239_t& lhs, const uint239_t& rhs){
    int shift_lhs = GetShift(lhs);
    int shift_rhs = GetShift(rhs);
    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);

    for (int i = 34; i != 0; i--){
        uint8_t n1 = reset_lhs.data[i];
        uint8_t n2 = reset_rhs.data[i];
        if (n1 == 0 && n2 == 0){
            continue;
        }
        if (n1 > n2) {
            return false;
        }
    }
    return true;

}

bool operator>(const uint239_t& lhs, const uint239_t& rhs) {
    int shift_lhs = GetShift(lhs);
    int shift_rhs = GetShift(rhs);
    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);
    for (int i = 0; i < 35; i++) {
        if (reset_lhs.data[i] + reset_rhs.data[i] == 0) {
            continue;
        }
        if ((reset_lhs.data[i] > reset_rhs.data[i])) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool operator<(const uint239_t& lhs, const uint239_t& rhs){
    int shift_lhs = GetShift(lhs);
    int shift_rhs = GetShift(rhs);
    uint239_t reset_lhs = FromShift(lhs, 245 - shift_lhs % 245, 0);
    uint239_t reset_rhs = FromShift(rhs, 245 - shift_rhs % 245, 0);

    for (int i = 34; i != 0; i--){
        uint8_t n1 = reset_lhs.data[i];
        uint8_t n2 = reset_rhs.data[i];
        if (n1 >= n2) {
            return false;
        }
    }
    return true;

}

std::ostream& operator<<(std::ostream& stream, const uint239_t& value) {
    for (int i = 0; i < 35; i++){
        int n = 128; // 2^7
        for (int j = 0; j < 8; j++){
            stream << (value.data[i] / n) % 2;
            n = n / 2;
        }
        stream << " ";
    }
    return stream;
}

uint64_t GetShift(const uint239_t& value) {
    uint64_t shift = 0;
    uint64_t carry = 1;
    for (int i = 34; i >= 0; i--) {
        if ((value.data[i] >> 7) == 1) {
            shift += carry;
        }
        carry *= 2;
    }
    return shift;
}

uint239_t DivideByTwo(const uint239_t& value){
    uint239_t result = value;
    int carry = 0;
    for (int i = 0; i <= 34; i++){
        result.data[i] = result.data[i] + carry * 128;
        carry = result.data[i] % 2;
        result.data[i] /= 2;
    }
    return result;
}

uint239_t FromShift(const uint239_t& value, uint32_t shift, int flag) {
    uint239_t result;
    int ind = 34 - (shift / 7) % 35;
    int shift_in_segment = shift % 7;
    uint16_t current = 0;
    uint16_t byte_end = 0;
    current = value.data[34] & 127;
    for (int i = 34; i > -1; i--) {
        if (ind < 0) {
            ind = 34;
        }
        if (i == 34) {
            result.data[ind] = current << shift_in_segment;
        } else {
            byte_end = ((value.data[i] & 127) << shift_in_segment);
            result.data[ind] = current | byte_end;
        }
        current = (value.data[i] & 127) >> (7 - shift_in_segment);
        result.data[ind] = result.data[ind] & 127;
        ind--;
    }
    if (shift < 7) {
        result.data[34] |= (value.data[0] & 127) >> (7 - shift_in_segment);
    }

    if (flag && shift != 0) {
        for (int i = 34; i >= 0; i--) {
            result.data[i] = result.data[i] | (shift << 7);
            shift = shift >> 1;
        }
    }
    return result;
}