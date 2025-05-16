//
// Created by janezp on 15/05/25.
//

#include "rlp.h"

char *gcvt(double x, int ndigit, char *buf);

static const char * _web3e_hexStr = "0123456789abcdef";
// returns output (header) length
uint32_t RLP::RlpEncodeWholeHeader(uint8_t* header_output, uint32_t total_len) {
    if (total_len < 55) {
        header_output[0] = (uint8_t)0xc0 + (uint8_t)total_len;
        return 1;
    } else {
        uint8_t tmp_header[8];
        memset(tmp_header, 0, 8);
        uint32_t hexdigit = 1;
        uint32_t tmp = total_len;
        while ((uint32_t)(tmp / 256) > 0) {
            tmp_header[hexdigit] = (uint8_t)(tmp % 256);
            tmp = (uint32_t)(tmp / 256);
            hexdigit++;
        }
        tmp_header[hexdigit] = (uint8_t)(tmp);
        tmp_header[0] = (uint8_t)0xf7 + (uint8_t)hexdigit;

        // fix direction for header
        uint8_t header[8];
        memset(header, 0, 8);
        header[0] = tmp_header[0];
        for (int i=0; i<hexdigit; i++) {
            header[i+1] = tmp_header[hexdigit-i];
        }

        memcpy(header_output, header, (size_t)hexdigit+1);
        return hexdigit+1;
    }
}

vector<uint8_t> RLP::RlpEncodeWholeHeaderWithVector(uint32_t total_len) {
    vector<uint8_t> header_output;
    if (total_len < 55) {
        header_output.push_back((uint8_t)0xc0 + (uint8_t)total_len);
    } else {
        vector<uint8_t> tmp_header;
        uint32_t hexdigit = 1;
        uint32_t tmp = total_len;
        while ((uint32_t)(tmp / 256) > 0) {
            tmp_header.push_back((uint8_t)(tmp % 256));
            tmp = (uint32_t)(tmp / 256);
            hexdigit++;
        }
        tmp_header.push_back((uint8_t)(tmp));
        tmp_header.insert(tmp_header.begin(), 0xf7 + (uint8_t)hexdigit);

        // fix direction for header
        vector<uint8_t> header;
        header.push_back(tmp_header[0]);
        for (int i=0; i<tmp_header.size()-1; i++) {
            header.push_back(tmp_header[tmp_header.size()-1-i]);
        }

        header_output.insert(header_output.end(), header.begin(), header.end());
    }
    return header_output;
}


// returns output length
uint32_t RLP::RlpEncodeItem(uint8_t* output, const uint8_t* input, uint32_t input_len) {
    if (input_len==1 && input[0] == 0x00) {
        uint8_t c[1] = {0x80};
        memcpy(output, c, 1);
        return 1;
    } else if (input_len==1 && input[0] < 128) {
        memcpy(output, input, 1);
        return 1;
    } else if (input_len <= 55) {
        uint8_t _ = (uint8_t)0x80 + (uint8_t)input_len;
        uint8_t header[] = {_};
        memcpy(output, header, 1);
        memcpy(output+1, input, (size_t)input_len);
        return input_len+1;
    } else {
        uint8_t tmp_header[8];
        memset(tmp_header, 0, 8);
        uint32_t hexdigit = 1;
        uint32_t tmp = input_len;
        while ((uint32_t)(tmp / 256) > 0) {
            tmp_header[hexdigit] = (uint8_t)(tmp % 256);
            tmp = (uint32_t)(tmp / 256);
            hexdigit++;
        }
        tmp_header[hexdigit] = (uint8_t)(tmp);
        tmp_header[0] = (uint8_t)0xb7 + (uint8_t)hexdigit;

        // fix direction for header
        uint8_t header[8];
        memset(header, 0, 8);
        header[0] = tmp_header[0];
        for (int i=0; i<hexdigit; i++) {
            header[i+1] = tmp_header[hexdigit-i];
        }

        memcpy(output, header, hexdigit+1);
        memcpy(output+hexdigit+1, input, (size_t)input_len);
        return input_len+hexdigit+1;
    }
}

vector<uint8_t> RLP::RlpEncodeItemWithVector(const vector<uint8_t> input) {
    vector<uint8_t> output;
    uint16_t input_len = input.size();

    if (input_len==1 && input[0] == 0x00) {
        output.push_back(0x80);
    } else if (input_len==1 && input[0] < 128) {
        output.insert(output.end(), input.begin(), input.end());
    } else if (input_len <= 55) {
        uint8_t _ = (uint8_t)0x80 + (uint8_t)input_len;
        output.push_back(_);
        output.insert(output.end(), input.begin(), input.end());
    } else {
        vector<uint8_t> tmp_header;
        uint32_t tmp = input_len;
        while ((uint32_t)(tmp / 256) > 0) {
            tmp_header.push_back((uint8_t)(tmp % 256));
            tmp = (uint32_t)(tmp / 256);
        }
        tmp_header.push_back((uint8_t)(tmp));
        uint8_t len = tmp_header.size();// + 1;
        tmp_header.insert(tmp_header.begin(), 0xb7 + len);

        // fix direction for header
        vector<uint8_t> header;
        header.push_back(tmp_header[0]);
        uint8_t hexdigit = tmp_header.size() - 1;
        for (int i=0; i<hexdigit; i++) {
            header.push_back(tmp_header[hexdigit-i]);
        }

        output.insert(output.end(), header.begin(), header.end());
        output.insert(output.end(), input.begin(), input.end());
    }
    return output;
}

vector<uint8_t> RLP::ConvertNumberToVector(unsigned long long val) 
{
	vector<uint8_t> tmp;
	vector<uint8_t> ret;
	if ((unsigned long long)(val / 256) >= 0) {
		while ((unsigned long long)(val / 256) > 0) {
			tmp.push_back((uint8_t)(val % 256));
			val = (unsigned long long)(val / 256);
		}
		tmp.push_back((uint8_t)(val % 256));
		uint8_t len = tmp.size();
		for (int i = 0; i<len; i++) {
			ret.push_back(tmp[len - i - 1]);
		}
	}
	else {
		ret.push_back((uint8_t)val);
	}
	return ret;
}

vector<uint8_t> RLP::ConvertNumberToVector(uint32_t val) {
    return ConvertNumberToVector((unsigned long long) val);
}

uint32_t RLP::ConvertNumberToUintArray(uint8_t *str, uint32_t val) {
    uint32_t ret = 0;
    uint8_t tmp[8];
    memset(tmp,0,8);
    if ((uint32_t)(val / 256) >= 0) {
        while ((uint32_t)(val / 256) > 0) {
            tmp[ret] = (uint8_t)(val % 256);
            val = (uint32_t)(val / 256);
            ret++;
        }
        tmp[ret] = (uint8_t)(val % 256);
        for (int i=0; i<ret+1; i++) {
            str[i] = tmp[ret-i];
        }
    } else {
        str[0] = (uint8_t)val;
    }

    return ret+1;
}

uint8_t RLP::ConvertCharToByte(const uint8_t* ptr)
{
	char c[3];
	c[0] = *(ptr);
	c[1] = *(ptr + 1);
	c[2] = 0x00;
	return strtol(c, nullptr, 16);
}

vector<uint8_t> RLP::ConvertHexToVector(const uint8_t *in) 
{
    const uint8_t *ptr = in;
    vector<uint8_t> out;
    if (ptr[0] == '0' && ptr[1] == 'x') ptr += 2;

	size_t lenstr = strlen((const char*)ptr);
	int i = 0;
	if ((lenstr % 2) == 1) //deal with odd sized hex Strings
	{
		char c[2];
		c[0] = *ptr;
		c[1] = 0;
		out.push_back(ConvertCharToByte((const uint8_t*)c));
		i = 1;
	}
	for (; i<lenstr; i += 2)
	{
		out.push_back(ConvertCharToByte(ptr + i));
	}
	return out;
}

vector<uint8_t> RLP::ConvertStringToVector(const String* str)
{
    const uint8_t *ptr = (uint8_t*)str->c_str();
    vector<uint8_t> out;

    size_t lenstr = strlen((const char*)ptr);
    for (int i=0; i<lenstr; i+=2) {
        char c[3];
        c[0] = *(ptr+i);
        c[1] = *(ptr+i+1);
        c[2] = 0x00;
        uint8_t val = strtol(c, nullptr, 16);
        out.push_back(ConvertCharToByte(ptr + i));
    }
    return out;
}

vector<uint8_t> RLP::ConvertHexToVector(const String* str) {
    return ConvertHexToVector((uint8_t*)(str->c_str()));
}

uint32_t RLP::ConvertCharStrToUintArray(uint8_t *out, const uint8_t *in) {
    uint32_t ret = 0;
    const uint8_t *ptr = in;
    // remove "0x"
    if (in[0] == '0' && in[1] == 'x') ptr += 2;

    size_t lenstr = strlen((const char*)ptr);
    for (int i=0; i<lenstr; i+=2) {
        char c[3];
        c[0] = *(ptr+i);
        c[1] = *(ptr+i+1);
        c[2] = 0x00;
        uint8_t val = strtol(c, nullptr, 16);
        out[ret] = val;
        ret++;
    }
    return ret;
};

uint8_t RLP::HexToInt(uint8_t s) {
    uint8_t ret = 0;
    if(s >= '0' && s <= '9'){
        ret = uint8_t(s - '0');
    } else if(s >= 'a' && s <= 'f'){
        ret = uint8_t(s - 'a' + 10);
    } else if(s >= 'A' && s <= 'F'){
        ret = uint8_t(s - 'A' + 10);
    }
    return ret;
}

String RLP::VectorToString(const vector<uint8_t> *buf) 
{
    return ConvertBytesToHex((const uint8_t*)buf->data(), buf->size());
}

String RLP::ConvertIntegerToBytes(const int32_t value)
{
    size_t hex_len = 4 << 1;
    String rc(hex_len, '0');
    for (size_t i = 0, j = (hex_len - 1) * 4; i<hex_len; ++i, j -= 4)
    {
        rc[i] = _web3e_hexStr[(value >> j) & 0x0f];
    }
    return rc;
}

String RLP::PlainVectorToString(const vector<uint8_t> *buf)
{
	char *buffer = (char*) alloca(buf->size() * 2 + 1);
	char *pout = buffer;
	for (int i = 0; i < buf->size(); i++)
	{
		*pout++ = _web3e_hexStr[((*buf)[i] >> 4) & 0xF];
		*pout++ = _web3e_hexStr[(*buf)[i] & 0xF];
	}
	*pout = 0;
	return String(buffer);
}

String RLP::ConvertBytesToHex(const uint8_t *bytes, int length)
{
    char *buffer = (char*)alloca(length * 2 + 3);
    char *pout = buffer;
    *pout++ = '0';
    *pout++ = 'x';
    for (int i = 0; i < length; i++)
    {
        *pout++ = _web3e_hexStr[((bytes)[i] >> 4) & 0xF];
        *pout++ = _web3e_hexStr[(bytes)[i] & 0xF];
    }
    *pout = 0;
    return String(buffer);
}

void RLP::ConvertHexToBytes(uint8_t *_dst, const char *_src, int length)
{
    if (_src[0] == '0' && _src[1] == 'x') _src += 2; //chop off 0x

    for (int i = 0; i < length; i++)
    {
        char a = _src[2 * i];
        char b = _src[2 * i + 1];
        uint8_t extract = HexToInt(a) << 4 | HexToInt(b);
        _dst[i] = extract;
    }
}


String RLP::ConvertHexToASCII(const char *result, size_t length)
{
	//convert hex to String.
	//first trim all the zeros
	int index = 0;
	String converted = "";
	char reader;
	int state = 0;
	bool endOfString = false;

	//No ASCII is less than 16 so this is safe
	while (index < length && (result[index] == '0' || result[index] == 'x')) index++;

	while (index < length && endOfString == false)
	{
		// convert from hex to ascii
		char c = result[index];
		switch (state)
		{
		case 0:
			reader = (char)(RLP::HexToInt(c) * 16);
			state = 1;
			break;
		case 1:
			reader += (char)RLP::HexToInt(c);
			if (reader == 0)
			{
				endOfString = true;
			}
			else
			{
				converted += reader;
				state = 0;
			}
			break;
		}
		index++;
	}

	return converted;  
}

/**
 * Build a std::vector of bytes32 as hex Strings
 **/
vector<String>* RLP::ConvertCharStrToVector32(const char *resultPtr, size_t resultSize, vector<String> *result) 
{
	if (resultSize < 64) return result;
    if (resultPtr[0] == '0' && resultPtr[1] == 'x') resultPtr += 2;
	//estimate size of return
	int returnSize = resultSize / 64;
	result->reserve(returnSize);
    int index = 0;
    char element[65];
    element[64] = 0;

    while (index <= (resultSize - 64))
    {
        memcpy(element, resultPtr, 64);
        result->push_back(String(element));
        resultPtr += 64;
        index += 64;
    }

	return result;
}

String RLP::InterpretStringResult(const char *result)
{
    //convert to vector bytes32
    String retVal = "";

    if (result != NULL && strlen(result) > 0) 
    {
        vector<String> breakDown;
        RLP::ConvertCharStrToVector32(result, strlen(result), &breakDown);

        if (breakDown.size() > 2)
        {
            //check first value
            auto itr = breakDown.begin();
            long dyn = strtol(itr++->c_str(), NULL, 16);
            if (dyn == 32) //array marker
            {
                long length = strtol(itr++->c_str(), NULL, 16);
                //now get a pointer to String immediately after the length marker
                const char *strPtr = result + 2 + (2*64);
                retVal = ConvertHexToASCII(strPtr, length*2);
            }
        }
    }

    return retVal;
}

vector<uint8_t> RLP::RlpEncodeString(const String* str){
    String hex = RLP::ConvertBytesToHex(reinterpret_cast<const uint8_t *>(str->c_str()),
                                                 str->length());
    vector<uint8_t> vec = RLP::ConvertHexToVector(&hex);
    vector<uint8_t> output = RLP::RlpEncodeItemWithVector(vec);
    return output;
}
