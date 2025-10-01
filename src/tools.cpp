////////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////
#include "otpch.h"
#include "tools.h"

#include <iostream>
#include <iomanip>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include "vocation.h"
#include "configmanager.h"

extern ConfigManager g_config;

std::string transformToMD5(std::string plainText, bool upperCase)
{
	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[MD5_DIGEST_LENGTH];
	MD5_Final(md, &c);

	char output[MD5_DIGEST_LENGTH * 2 + 1] = "";
	for(int32_t i = 0; i < static_cast<int32_t>(sizeof(md)); i++)
		sprintf(output, "%s%.2X", output, md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

std::string transformToSHA1(std::string plainText, bool upperCase)
{
	SHA_CTX c;
	SHA1_Init(&c);
	SHA1_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[SHA_DIGEST_LENGTH];
	SHA1_Final(md, &c);

	char output[SHA_DIGEST_LENGTH * 2 + 1] = "";
	for(int32_t i = 0; i < static_cast<int32_t>(sizeof(md)); i++)
		sprintf(output, "%s%.2X", output, md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

std::string transformToSHA256(std::string plainText, bool upperCase)
{
	SHA256_CTX c;
	SHA256_Init(&c);
	SHA256_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[SHA256_DIGEST_LENGTH];
	SHA256_Final(md, &c);

	char output[SHA256_DIGEST_LENGTH * 2 + 1] = "";
	for(int32_t i = 0; i < static_cast<int32_t>(sizeof(md)); i++)
		sprintf(output, "%s%.2X", output, md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

std::string transformToSHA512(std::string plainText, bool upperCase)
{
	SHA512_CTX c;
	SHA512_Init(&c);
	SHA512_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[SHA512_DIGEST_LENGTH];
	SHA512_Final(md, &c);

	char output[SHA512_DIGEST_LENGTH * 2 + 1] = "";
	for(int32_t i = 0; i < static_cast<int32_t>(sizeof(md)); i++)
		sprintf(output, "%s%.2X", output, md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

void _encrypt(std::string& str, bool upperCase)
{
	switch(g_config.getNumber(ConfigManager::ENCRYPTION))
	{
		case ENCRYPTION_MD5:
			str = transformToMD5(str, upperCase);
			break;
		case ENCRYPTION_SHA1:
			str = transformToSHA1(str, upperCase);
			break;
		case ENCRYPTION_SHA256:
			str = transformToSHA256(str, upperCase);
			break;
		case ENCRYPTION_SHA512:
			str = transformToSHA512(str, upperCase);
			break;
		default:
		{
			if(upperCase)
				std::transform(str.begin(), str.end(), str.begin(), upchar);

			break;
		}
	}
}

bool encryptTest(std::string plain, std::string& hash)
{
	std::transform(hash.begin(), hash.end(), hash.begin(), upchar);
	_encrypt(plain, true);
	return plain == hash;
}

bool replaceString(std::string& text, const std::string& key, const std::string& value)
{
	if(text.find(key) == std::string::npos)
		return false;

	std::string::size_type start = 0, pos = 0;
	while((start = text.find(key, pos)) != std::string::npos)
	{
		text.replace(start, key.size(), value);
		//text = text.substr(0, start) + value + text.substr(start + key.size());
		pos = start + value.size();
	}

	return true;
}

void trim_right(std::string& source, const std::string& t)
{
	source.erase(source.find_last_not_of(t) + 1);
}

void trim_left(std::string& source, const std::string& t)
{
	source.erase(0, source.find_first_not_of(t));
}

void toLowerCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), tolower);
}

void toUpperCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), upchar);
}

std::string asLowerCaseString(const std::string& source)
{
	std::string s = source;
	toLowerCaseString(s);
	return s;
}

std::string asUpperCaseString(const std::string& source)
{
	std::string s = source;
	toUpperCaseString(s);
	return s;
}

bool booleanString(std::string source)
{
	toLowerCaseString(source);
	return (source == "yes" || source == "true" || atoi(source.c_str()) > 0);
}

bool readXMLInteger(xmlNodePtr node, const char* tag, int32_t& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	value = atoi(nodeValue);
	xmlFree(nodeValue);
	return true;
}

bool readXMLInteger64(xmlNodePtr node, const char* tag, int64_t& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	value = atoll(nodeValue);
	xmlFree(nodeValue);
	return true;
}

bool readXMLFloat(xmlNodePtr node, const char* tag, float& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	value = atof(nodeValue);
	xmlFree(nodeValue);
	return true;
}

bool readXMLString(xmlNodePtr node, const char* tag, std::string& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	if(!utf8ToLatin1(nodeValue, value))
		value = nodeValue;

	xmlFree(nodeValue);
	return true;
}

bool readXMLContentString(xmlNodePtr node, std::string& value)
{
	char* nodeValue = (char*)xmlNodeGetContent(node);
	if(!nodeValue)
		return false;

	if(!utf8ToLatin1(nodeValue, value))
		value = nodeValue;

	xmlFree(nodeValue);
	return true;
}

bool parseXMLContentString(xmlNodePtr node, std::string& value)
{
	bool result = false;
	std::string compareValue;
	while(node)
	{
		if(xmlStrcmp(node->name, (const xmlChar*)"text") && node->type != XML_CDATA_SECTION_NODE)
		{
			node = node->next;
			continue;
		}

		if(!readXMLContentString(node, compareValue))
		{
			node = node->next;
			continue;
		}

		trim_left(compareValue, "\r");
		trim_left(compareValue, "\n");
		trim_left(compareValue, " ");
		if(compareValue.length() > value.length())
		{
			value = compareValue;
			if(!result)
				result = true;
		}

		node = node->next;
	}

	return result;
}

std::string getLastXMLError()
{
	std::stringstream ss;
	xmlErrorPtr lastError = xmlGetLastError();
	if(lastError->line)
		ss << "Line: " << lastError->line << ", ";

	ss << "Info: " << lastError->message << std::endl;
	return ss.str();
}

bool utf8ToLatin1(char* intext, std::string& outtext)
{
	outtext = "";
	if(!intext)
		return false;

	int32_t inlen = strlen(intext);
	if(!inlen)
		return false;

	int32_t outlen = inlen * 2;
	uint8_t* outbuf = new uint8_t[outlen];

	int32_t res = UTF8Toisolat1(outbuf, &outlen, (uint8_t*)intext, &inlen);
	if(res < 0)
	{
		delete[] outbuf;
		return false;
	}

	outbuf[outlen] = '\0';
	outtext = (char*)outbuf;

	delete[] outbuf;
	return true;
}

StringVec explodeString(const std::string& string, const std::string& separator)
{
	StringVec returnVector;
	size_t start = 0, end = 0;
	while((end = string.find(separator, start)) != std::string::npos)
	{
		returnVector.push_back(string.substr(start, end - start));
		start = end + separator.size();
	}

	returnVector.push_back(string.substr(start));
	return returnVector;
}

IntegerVec vectorAtoi(StringVec stringVector)
{
	IntegerVec returnVector;
	for(StringVec::iterator it = stringVector.begin(); it != stringVector.end(); ++it)
		returnVector.push_back(atoi((*it).c_str()));

	return returnVector;
}

bool hasBitSet(uint32_t flag, uint32_t flags)
{
	return ((flags & flag) == flag);
}

int32_t round(float v)
{
	int32_t t = (int32_t)std::floor(v);
	if((v - t) > 0.5)
		return t + 1;

	return t;
}

uint32_t rand24b()
{
	return ((rand() << 12) ^ (rand())) & (0xFFFFFF);
}

float box_muller(float m, float s)
{
	// normal random variate generator
	// mean m, standard deviation s
	float x1, x2, w, y1;
	static float y2;

	static bool useLast = false;
	if(useLast) // use value from previous call
	{
		y1 = y2;
		useLast = false;
		return (m + y1 * s);
	}

	do
	{
		double r1 = (((float)(rand()) / RAND_MAX));
		double r2 = (((float)(rand()) / RAND_MAX));

		x1 = 2.0 * r1 - 1.0;
		x2 = 2.0 * r2 - 1.0;
		w = x1 * x1 + x2 * x2;
	}
	while(w >= 1.0);
	w = sqrt((-2.0 * log(w)) / w);

	y1 = x1 * w;
	y2 = x2 * w;

	useLast = true;
	return (m + y1 * s);
}

int32_t random_range(int32_t lowestNumber, int32_t highestNumber, DistributionType_t type /*= DISTRO_UNIFORM*/)
{
	if(highestNumber == lowestNumber)
		return lowestNumber;

	if(lowestNumber > highestNumber)
		std::swap(lowestNumber, highestNumber);

	switch(type)
	{
		case DISTRO_UNIFORM:
			return (lowestNumber + ((int32_t)rand24b() % (highestNumber - lowestNumber + 1)));
		case DISTRO_NORMAL:
			return (lowestNumber + int32_t(float(highestNumber - lowestNumber) * (float)std::min((float)1, std::max((float)0, box_muller(0.5, 0.25)))));
		default:
			break;
	}

	const float randMax = 16777216;
	return (lowestNumber + int32_t(float(highestNumber - lowestNumber) * float(1.f - sqrt((1.f * rand24b()) / randMax))));
}

char upchar(char character)
{
	if((character >= 97 && character <= 122) || (character <= -1 && character >= -32))
		character -= 32;

	return character;
}

bool isNumber(char character)
{
	return (character >= 48 && character <= 57);
}

bool isLowercaseLetter(char character)
{
	return (character >= 97 && character <= 122);
}

bool isUppercaseLetter(char character)
{
	return (character >= 65 && character <= 90);
}

bool isPasswordCharacter(char character)
{
	return ((character >= 33 && character <= 47) || (character >= 58 && character <= 64) || (character >= 91 && character <= 96) || (character >= 123 && character <= 126));
}

bool isValidAccountName(std::string text)
{
	toLowerCaseString(text);

	uint32_t textLength = text.length();
	for(uint32_t size = 0; size < textLength; size++)
	{
		if(!isLowercaseLetter(text[size]) && !isNumber(text[size]))
			return false;
	}

	return true;
}

bool isValidPassword(std::string text)
{
	toLowerCaseString(text);

	uint32_t textLength = text.length();
	for(uint32_t size = 0; size < textLength; size++)
	{
		if(!isLowercaseLetter(text[size]) && !isNumber(text[size]) && !isPasswordCharacter(text[size]))
			return false;
	}

	return true;
}

bool isValidName(std::string text, bool forceUppercaseOnFirstLetter/* = true*/)
{
	uint32_t textLength = text.length(), lenBeforeSpace = 1, lenBeforeQuote = 1, lenBeforeDash = 1, repeatedCharacter = 0;
	char lastChar = 32;
	if(forceUppercaseOnFirstLetter)
	{
		if(!isUppercaseLetter(text[0]))
			return false;
	}
	else if(!isLowercaseLetter(text[0]) && !isUppercaseLetter(text[0]))
		return false;

	for(uint32_t size = 1; size < textLength; size++)
	{
		if(text[size] != 32)
		{
			lenBeforeSpace++;
			if(text[size] != 39)
				lenBeforeQuote++;
			else
			{
				if(lenBeforeQuote <= 1 || size == textLength - 1 || text[size + 1] == 32)
					return false;

				lenBeforeQuote = 0;
			}

			if(text[size] != 45)
				lenBeforeDash++;
			else
			{
				if(lenBeforeDash <= 1 || size == textLength - 1 || text[size + 1] == 32)
					return false;

				lenBeforeDash = 0;
			}

			if(text[size] == lastChar)
			{
				repeatedCharacter++;
				if(repeatedCharacter > 2)
					return false;
			}
			else
				repeatedCharacter = 0;

			lastChar = text[size];
		}
		else
		{
			if(lenBeforeSpace <= 1 || size == textLength - 1 || text[size + 1] == 32)
				return false;

			lenBeforeSpace = lenBeforeQuote = lenBeforeDash = 0;
		}

		if(!(isLowercaseLetter(text[size]) || text[size] == 32 || text[size] == 39 || text[size] == 45
			|| (isUppercaseLetter(text[size]) && text[size - 1] == 32)))
			return false;
	}

	return true;
}

bool isNumbers(std::string text)
{
	uint32_t textLength = text.length();
	for(uint32_t size = 0; size < textLength; size++)
	{
		if(!isNumber(text[size]))
			return false;
	}

	return true;
}

bool checkText(std::string text, std::string str)
{
	trimString(text);
	return asLowerCaseString(text) == str;
}

std::string generateRecoveryKey(int32_t fieldCount, int32_t fieldLenght, bool mixCase/* = false*/)
{
	std::stringstream key;
	int32_t i = 0, j = 0, lastNumber = 99, number = 0;

	char character = 0, lastCharacter = 0;
	bool madeNumber = false, madeCharacter = false;
	do
	{
		do
		{
			madeNumber = madeCharacter = false;
			if((mixCase && !random_range(0, 2)) || (!mixCase && (bool)random_range(0, 1)))
			{
				number = random_range(2, 9);
				if(number != lastNumber)
				{
					key << number;
					lastNumber = number;
					madeNumber = true;
				}
			}
			else
			{
				if(mixCase && (bool)random_range(0,1) )
					character = (char)random_range(97, 122);
				else
					character = (char)random_range(65, 90);

				if(character != lastCharacter)
				{
					key << character;
					lastCharacter = character;
					madeCharacter = true;
				}
			}
		}
		while((!madeCharacter && !madeNumber) ? true : (++j && j < fieldLenght));
		lastCharacter = character = number = j = 0;

		lastNumber = 99;
		if(i < fieldCount - 1)
			key << "-";
	}
	while(++i && i < fieldCount);
	return key.str();
}

std::string trimString(std::string& str)
{
	str.erase(str.find_last_not_of(" ") + 1);
	return str.erase(0, str.find_first_not_of(" "));
}

std::string parseParams(tokenizer::iterator &it, tokenizer::iterator end)
{
	if(it == end)
		return "";

	std::string tmp = (*it);
	++it;
	if(tmp[0] == '"')
	{
		tmp.erase(0, 1);
		while(it != end && tmp[tmp.length() - 1] != '"')
		{
			tmp += " " + (*it);
			++it;
		}

		if(tmp.length() > 0 && tmp[tmp.length() - 1] == '"')
			tmp.erase(tmp.length() - 1);
	}

	return tmp;
}

std::string formatDate(time_t _time/* = 0*/)
{
	if(!_time)
		_time = time(NULL);

	const tm* tms = localtime(&_time);
	std::stringstream s;
	if(tms)
		s << tms->tm_mday << "/" << (tms->tm_mon + 1) << "/" << (tms->tm_year + 1900) << " " << tms->tm_hour << ":" << tms->tm_min << ":" << tms->tm_sec;
	else
		s << "UNIX Time: " << (int32_t)_time;

	return s.str();
}

std::string formatDateEx(time_t _time/* = 0*/, std::string format/* = "%d %b %Y, %H:%M:%S"*/)
{
	if(!_time)
		_time = time(NULL);

	const tm* tms = localtime(&_time);
	char buffer[100];
	if(tms)
		strftime(buffer, 25, format.c_str(), tms);
	else
		sprintf(buffer, "UNIX Time: %d", (int32_t)_time);

	return buffer;
}

std::string formatTime(time_t _time/* = 0*/, bool ms/* = false*/)
{
	if(!_time)
		_time = time(NULL);
	else if(ms)
		ms = false;

	const tm* tms = localtime(&_time);
	std::stringstream s;
	if(tms)
	{
		s << tms->tm_hour << ":" << tms->tm_min << ":";
		if(tms->tm_sec < 10)
			s << "0";

		s << tms->tm_sec;
		if(ms)
		{
			timeb t;
			ftime(&t);

			s << "."; // make it format zzz
			if(t.millitm < 10)
				s << "0";

			if(t.millitm < 100)
				s << "0";

			s << t.millitm;
		}
	}
	else
		s << "UNIX Time: " << (int32_t)_time;

	return s.str();
}

std::string convertIPAddress(uint32_t ip)
{
	char buffer[17];
	sprintf(buffer, "%d.%d.%d.%d", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24));
	return buffer;
}

Skulls_t getSkulls(std::string strValue)
{
	std::string tmpStrValue = asLowerCaseString(strValue);
	if(tmpStrValue == "SKULL_1" || tmpStrValue == "6")
		return SKULL_1;
	if(tmpStrValue == "SKULL_2" || tmpStrValue == "7")
		return SKULL_2;
	if(tmpStrValue == "SKULL_3" || tmpStrValue == "8")
		return SKULL_3;
	if(tmpStrValue == "SKULL_4" || tmpStrValue == "9")
		return SKULL_4;
	if(tmpStrValue == "SKULL_5" || tmpStrValue == "10")
		return SKULL_5;
	if(tmpStrValue == "SKULL_6" || tmpStrValue == "11")
		return SKULL_6;
	if(tmpStrValue == "SKULL_7" || tmpStrValue == "12")
		return SKULL_7;
	if(tmpStrValue == "SKULL_8" || tmpStrValue == "13")
		return SKULL_8;
	if(tmpStrValue == "SKULL_9" || tmpStrValue == "14")
		return SKULL_9;
	if(tmpStrValue == "SKULL_10" || tmpStrValue == "15")
		return SKULL_10;
	if(tmpStrValue == "SKULL_11" || tmpStrValue == "16")
		return SKULL_11;
	if(tmpStrValue == "SKULL_12" || tmpStrValue == "17")
		return SKULL_12;
	if(tmpStrValue == "SKULL_13" || tmpStrValue == "18")
		return SKULL_13;
	if(tmpStrValue == "SKULL_14" || tmpStrValue == "19")
		return SKULL_14;
	if(tmpStrValue == "SKULL_15" || tmpStrValue == "20")
		return SKULL_15;
	if(tmpStrValue == "SKULL_16" || tmpStrValue == "21")
		return SKULL_16;
	if(tmpStrValue == "SKULL_17" || tmpStrValue == "22")
		return SKULL_17;
	if(tmpStrValue == "SKULL_18" || tmpStrValue == "23")
		return SKULL_18;
	if(tmpStrValue == "SKULL_19" || tmpStrValue == "24")
		return SKULL_19;
	if(tmpStrValue == "SKULL_20" || tmpStrValue == "25")
		return SKULL_20;
	if(tmpStrValue == "SKULL_21" || tmpStrValue == "26")
		return SKULL_21;
	if(tmpStrValue == "SKULL_22" || tmpStrValue == "27")
		return SKULL_22;
	if(tmpStrValue == "SKULL_23" || tmpStrValue == "28")
		return SKULL_23;
	if(tmpStrValue == "SKULL_24" || tmpStrValue == "29")
		return SKULL_24;
	if(tmpStrValue == "SKULL_25" || tmpStrValue == "30")
		return SKULL_25;
	if(tmpStrValue == "SKULL_26" || tmpStrValue == "31")
		return SKULL_26;
	if(tmpStrValue == "SKULL_27" || tmpStrValue == "32")
		return SKULL_27;
	if(tmpStrValue == "SKULL_28" || tmpStrValue == "33")
		return SKULL_28;
	if(tmpStrValue == "SKULL_29" || tmpStrValue == "34")
		return SKULL_29;
	if(tmpStrValue == "SKULL_30" || tmpStrValue == "35")
		return SKULL_30;
	if(tmpStrValue == "SKULL_31" || tmpStrValue == "36")
		return SKULL_31;
	if(tmpStrValue == "SKULL_32" || tmpStrValue == "37")
		return SKULL_32;
	if(tmpStrValue == "SKULL_33" || tmpStrValue == "38")
		return SKULL_33;
	if(tmpStrValue == "SKULL_34" || tmpStrValue == "39")
		return SKULL_34;
	if(tmpStrValue == "SKULL_35" || tmpStrValue == "40")
		return SKULL_35;
	if(tmpStrValue == "SKULL_36" || tmpStrValue == "41")
		return SKULL_36;
	if(tmpStrValue == "SKULL_37" || tmpStrValue == "42")
		return SKULL_37;
	if(tmpStrValue == "SKULL_38" || tmpStrValue == "43")
		return SKULL_38;
	if(tmpStrValue == "SKULL_39" || tmpStrValue == "44")
		return SKULL_39;
	if(tmpStrValue == "SKULL_40" || tmpStrValue == "45")
		return SKULL_40;
	if(tmpStrValue == "SKULL_41" || tmpStrValue == "46")
		return SKULL_41;
	if(tmpStrValue == "SKULL_42" || tmpStrValue == "47")
		return SKULL_42;
	if(tmpStrValue == "SKULL_43" || tmpStrValue == "48")
		return SKULL_43;
	if(tmpStrValue == "SKULL_44" || tmpStrValue == "49")
		return SKULL_44;
	if(tmpStrValue == "SKULL_45" || tmpStrValue == "50")
		return SKULL_45;
	if(tmpStrValue == "SKULL_46" || tmpStrValue == "51")
		return SKULL_46;
	if(tmpStrValue == "SKULL_47" || tmpStrValue == "52")
		return SKULL_47;
	if(tmpStrValue == "SKULL_48" || tmpStrValue == "53")
		return SKULL_48;
	if(tmpStrValue == "SKULL_49" || tmpStrValue == "54")
		return SKULL_49;
	if(tmpStrValue == "SKULL_50" || tmpStrValue == "55")
		return SKULL_50;
	if(tmpStrValue == "SKULL_51" || tmpStrValue == "56")
		return SKULL_51;
	if(tmpStrValue == "SKULL_52" || tmpStrValue == "57")
		return SKULL_52;
	if(tmpStrValue == "SKULL_53" || tmpStrValue == "58")
		return SKULL_53;
	if(tmpStrValue == "SKULL_54" || tmpStrValue == "59")
		return SKULL_54;
	if(tmpStrValue == "SKULL_55" || tmpStrValue == "60")
		return SKULL_55;
	if(tmpStrValue == "SKULL_56" || tmpStrValue == "61")
		return SKULL_56;
	if(tmpStrValue == "SKULL_57" || tmpStrValue == "62")
		return SKULL_57;
	if(tmpStrValue == "SKULL_58" || tmpStrValue == "63")
		return SKULL_58;
	if(tmpStrValue == "SKULL_59" || tmpStrValue == "64")
		return SKULL_59;
	if(tmpStrValue == "SKULL_60" || tmpStrValue == "65")
		return SKULL_60;
	if(tmpStrValue == "SKULL_61" || tmpStrValue == "66")
		return SKULL_61;
	if(tmpStrValue == "SKULL_62" || tmpStrValue == "67")
		return SKULL_62;
	if(tmpStrValue == "SKULL_63" || tmpStrValue == "68")
		return SKULL_63;
	if(tmpStrValue == "SKULL_64" || tmpStrValue == "69")
		return SKULL_64;
	if(tmpStrValue == "SKULL_65" || tmpStrValue == "70")
		return SKULL_65;
	if(tmpStrValue == "SKULL_66" || tmpStrValue == "71")
		return SKULL_66;
	if(tmpStrValue == "SKULL_67" || tmpStrValue == "72")
		return SKULL_67;
	if(tmpStrValue == "SKULL_68" || tmpStrValue == "73")
		return SKULL_68;
	if(tmpStrValue == "SKULL_69" || tmpStrValue == "74")
		return SKULL_69;
	if(tmpStrValue == "SKULL_70" || tmpStrValue == "75")
		return SKULL_70;
	if(tmpStrValue == "SKULL_71" || tmpStrValue == "76")
		return SKULL_71;
	if(tmpStrValue == "SKULL_72" || tmpStrValue == "77")
		return SKULL_72;
	if(tmpStrValue == "SKULL_73" || tmpStrValue == "78")
		return SKULL_73;
	if(tmpStrValue == "SKULL_74" || tmpStrValue == "79")
		return SKULL_74;
	if(tmpStrValue == "SKULL_75" || tmpStrValue == "80")
		return SKULL_75;
	if(tmpStrValue == "SKULL_76" || tmpStrValue == "81")
		return SKULL_76;
	if(tmpStrValue == "SKULL_77" || tmpStrValue == "82")
		return SKULL_77;
	if(tmpStrValue == "SKULL_78" || tmpStrValue == "83")
		return SKULL_78;
	if(tmpStrValue == "SKULL_79" || tmpStrValue == "84")
		return SKULL_79;
	if(tmpStrValue == "SKULL_80" || tmpStrValue == "85")
		return SKULL_80;
	if(tmpStrValue == "SKULL_81" || tmpStrValue == "86")
		return SKULL_81;
	if(tmpStrValue == "SKULL_82" || tmpStrValue == "87")
		return SKULL_82;
	if(tmpStrValue == "SKULL_83" || tmpStrValue == "88")
		return SKULL_83;
	if(tmpStrValue == "SKULL_84" || tmpStrValue == "89")
		return SKULL_84;
	if(tmpStrValue == "SKULL_85" || tmpStrValue == "90")
		return SKULL_85;
	if(tmpStrValue == "SKULL_86" || tmpStrValue == "91")
		return SKULL_86;
	if(tmpStrValue == "SKULL_87" || tmpStrValue == "92")
		return SKULL_87;
	if(tmpStrValue == "SKULL_88" || tmpStrValue == "93")
		return SKULL_88;
	if(tmpStrValue == "SKULL_89" || tmpStrValue == "94")
		return SKULL_89;
	if(tmpStrValue == "SKULL_90" || tmpStrValue == "95")
		return SKULL_90;
	if(tmpStrValue == "SKULL_91" || tmpStrValue == "96")
		return SKULL_91;
	if(tmpStrValue == "SKULL_92" || tmpStrValue == "97")
		return SKULL_92;
	if(tmpStrValue == "SKULL_93" || tmpStrValue == "98")
		return SKULL_93;
	if(tmpStrValue == "SKULL_94" || tmpStrValue == "99")
		return SKULL_94;
	if(tmpStrValue == "SKULL_95" || tmpStrValue == "100")
		return SKULL_95;
	if(tmpStrValue == "SKULL_96" || tmpStrValue == "101")
		return SKULL_96;
	if(tmpStrValue == "SKULL_97" || tmpStrValue == "102")
		return SKULL_97;
	if(tmpStrValue == "SKULL_98" || tmpStrValue == "103")
		return SKULL_98;
	if(tmpStrValue == "SKULL_99" || tmpStrValue == "104")
		return SKULL_99;
	if(tmpStrValue == "SKULL_100" || tmpStrValue == "105")
		return SKULL_100;


	if(tmpStrValue == "black" || tmpStrValue == "5")
		return SKULL_BLACK;

	if(tmpStrValue == "red" || tmpStrValue == "4")
		return SKULL_RED;

	if(tmpStrValue == "white" || tmpStrValue == "3")
		return SKULL_WHITE;

	if(tmpStrValue == "green" || tmpStrValue == "2")
		return SKULL_GREEN;

	if(tmpStrValue == "yellow" || tmpStrValue == "1")
		return SKULL_YELLOW;

	return SKULL_NONE;
}

PartyShields_t getShields(std::string strValue)
{
	std::string tmpStrValue = asLowerCaseString(strValue);
	if(tmpStrValue == "whitenoshareoff" || tmpStrValue == "10")
		return SHIELD_YELLOW_NOSHAREDEXP;

	if(tmpStrValue == "blueshareoff" || tmpStrValue == "9")
		return SHIELD_BLUE_NOSHAREDEXP;

	if(tmpStrValue == "yellowshareblink" || tmpStrValue == "8")
		return SHIELD_YELLOW_NOSHAREDEXP_BLINK;

	if(tmpStrValue == "blueshareblink" || tmpStrValue == "7")
		return SHIELD_BLUE_NOSHAREDEXP_BLINK;

	if(tmpStrValue == "yellowshareon" || tmpStrValue == "6")
		return SHIELD_YELLOW_SHAREDEXP;

	if(tmpStrValue == "blueshareon" || tmpStrValue == "5")
		return SHIELD_BLUE_SHAREDEXP;

	if(tmpStrValue == "yellow" || tmpStrValue == "4")
		return SHIELD_YELLOW;

	if(tmpStrValue == "blue" || tmpStrValue == "3")
		return SHIELD_BLUE;

	if(tmpStrValue == "whiteyellow" || tmpStrValue == "2")
		return SHIELD_WHITEYELLOW;

	if(tmpStrValue == "whiteblue" || tmpStrValue == "1")
		return SHIELD_WHITEBLUE;

	return SHIELD_NONE;
}

GuildEmblems_t getEmblems(std::string strValue)
{
	std::string tmpStrValue = asLowerCaseString(strValue);
	if(tmpStrValue == "blue" || tmpStrValue == "3")
		return EMBLEM_BLUE;

	if(tmpStrValue == "red" || tmpStrValue == "2")
		return EMBLEM_RED;

	if(tmpStrValue == "green" || tmpStrValue == "1")
		return EMBLEM_GREEN;

	return EMBLEM_NONE;
}



Direction getDirection(std::string string)
{
	if(string == "north" || string == "n" || string == "0")
		return NORTH;

	if(string == "east" || string == "e" || string == "1")
		return EAST;

	if(string == "south" || string == "s" || string == "2")
		return SOUTH;

	if(string == "west" || string == "w" || string == "3")
		return WEST;

	if(string == "southwest" || string == "south west" || string == "south-west" || string == "sw" || string == "4")
		return SOUTHWEST;

	if(string == "southeast" || string == "south east" || string == "south-east" || string == "se" || string == "5")
		return SOUTHEAST;

	if(string == "northwest" || string == "north west" || string == "north-west" || string == "nw" || string == "6")
		return NORTHWEST;

	if(string == "northeast" || string == "north east" || string == "north-east" || string == "ne" || string == "7")
		return NORTHEAST;

	return SOUTH;
}

Direction getDirectionTo(Position pos1, Position pos2, bool extended/* = true*/)
{
	Direction direction = NORTH;
	if(pos1.x > pos2.x)
	{
		direction = WEST;
		if(extended)
		{
			if(pos1.y > pos2.y)
				direction = NORTHWEST;
			else if(pos1.y < pos2.y)
				direction = SOUTHWEST;
		}
	}
	else if(pos1.x < pos2.x)
	{
		direction = EAST;
		if(extended)
		{
			if(pos1.y > pos2.y)
				direction = NORTHEAST;
			else if(pos1.y < pos2.y)
				direction = SOUTHEAST;
		}
	}
	else
	{
		if(pos1.y > pos2.y)
			direction = NORTH;
		else if(pos1.y < pos2.y)
			direction = SOUTH;
	}

	return direction;
}

Direction getReverseDirection(Direction dir)
{
	switch(dir)
	{
		case NORTH:
			return SOUTH;
		case SOUTH:
			return NORTH;
		case WEST:
			return EAST;
		case EAST:
			return WEST;
		case SOUTHWEST:
			return NORTHEAST;
		case NORTHWEST:
			return SOUTHEAST;
		case NORTHEAST:
			return SOUTHWEST;
		case SOUTHEAST:
			return NORTHWEST;
	}

	return SOUTH;
}

Position getNextPosition(Direction direction, Position pos)
{
	switch(direction)
	{
		case NORTH:
			pos.y--;
			break;
		case SOUTH:
			pos.y++;
			break;
		case WEST:
			pos.x--;
			break;
		case EAST:
			pos.x++;
			break;
		case SOUTHWEST:
			pos.x--;
			pos.y++;
			break;
		case NORTHWEST:
			pos.x--;
			pos.y--;
			break;
		case SOUTHEAST:
			pos.x++;
			pos.y++;
			break;
		case NORTHEAST:
			pos.x++;
			pos.y--;
			break;
	}

	return pos;
}

struct AmmoTypeNames
{
	const char* name;
	Ammo_t ammoType;
};

struct MagicEffectNames
{
	const char* name;
	MagicEffect_t magicEffect;
};

struct ShootTypeNames
{
	const char* name;
	ShootEffect_t shootType;
};

struct CombatTypeNames
{
	const char* name;
	CombatType_t combatType;
};

struct AmmoActionNames
{
	const char* name;
	AmmoAction_t ammoAction;
};

struct FluidTypeNames
{
	const char* name;
	FluidTypes_t fluidType;
};

struct SkillIdNames
{
	const char* name;
	skills_t skillId;
};

MagicEffectNames magicEffectNames[] =
{
	{"redspark",		MAGIC_EFFECT_DRAW_BLOOD},
	{"bluebubble",		MAGIC_EFFECT_LOSE_ENERGY},
	{"poff",		MAGIC_EFFECT_POFF},
	{"yellowspark",		MAGIC_EFFECT_BLOCKHIT},
	{"explosionarea",	MAGIC_EFFECT_EXPLOSION_AREA},
	{"explosion",		MAGIC_EFFECT_EXPLOSION_DAMAGE},
	{"firearea",		MAGIC_EFFECT_FIRE_AREA},
	{"yellowbubble",	MAGIC_EFFECT_YELLOW_RINGS},
	{"greenbubble",		MAGIC_EFFECT_POISON_RINGS},
	{"blackspark",		MAGIC_EFFECT_HIT_AREA},
	{"teleport",		MAGIC_EFFECT_TELEPORT},
	{"energy",		MAGIC_EFFECT_ENERGY_DAMAGE},
	{"blueshimmer",		MAGIC_EFFECT_WRAPS_BLUE},
	{"redshimmer",		MAGIC_EFFECT_WRAPS_RED},
	{"greenshimmer",	MAGIC_EFFECT_WRAPS_GREEN},
	{"fire",		MAGIC_EFFECT_HITBY_FIRE},
	{"greenspark",		MAGIC_EFFECT_POISON},
	{"mortarea",		MAGIC_EFFECT_MORT_AREA},
	{"greennote",		MAGIC_EFFECT_SOUND_GREEN},
	{"rednote",		MAGIC_EFFECT_SOUND_RED},
	{"poison",		MAGIC_EFFECT_POISON_AREA},
	{"yellownote",		MAGIC_EFFECT_SOUND_YELLOW},
	{"purplenote",		MAGIC_EFFECT_SOUND_PURPLE},
	{"bluenote",		MAGIC_EFFECT_SOUND_BLUE},
	{"whitenote",		MAGIC_EFFECT_SOUND_WHITE},
	{"bubbles",		MAGIC_EFFECT_BUBBLES},
	{"dice",		MAGIC_EFFECT_CRAPS},
	{"giftwraps",		MAGIC_EFFECT_GIFT_WRAPS},
	{"yellowfirework",	MAGIC_EFFECT_FIREWORK_YELLOW},
	{"redfirework",		MAGIC_EFFECT_FIREWORK_RED},
	{"bluefirework",	MAGIC_EFFECT_FIREWORK_BLUE},
	{"stun",		MAGIC_EFFECT_STUN},
	{"sleep",		MAGIC_EFFECT_SLEEP},
	{"watercreature",	MAGIC_EFFECT_WATERCREATURE},
	{"groundshaker",	MAGIC_EFFECT_GROUNDSHAKER},
	{"hearts",		MAGIC_EFFECT_HEARTS},
	{"fireattack",		MAGIC_EFFECT_FIREATTACK},
	{"energyarea",		MAGIC_EFFECT_ENERGY_AREA},
	{"smallclouds",		MAGIC_EFFECT_SMALLCLOUDS},
	{"holydamage",		MAGIC_EFFECT_HOLYDAMAGE},
	{"bigclouds",		MAGIC_EFFECT_BIGCLOUDS},
	{"icearea",		MAGIC_EFFECT_ICEAREA},
	{"icetornado",		MAGIC_EFFECT_ICETORNADO},
	{"iceattack",		MAGIC_EFFECT_ICEATTACK},
	{"stones",		MAGIC_EFFECT_STONES},
	{"smallplants",		MAGIC_EFFECT_SMALLPLANTS},
	{"carniphila",		MAGIC_EFFECT_CARNIPHILA},
	{"purpleenergy",	MAGIC_EFFECT_PURPLEENERGY},
	{"yellowenergy",	MAGIC_EFFECT_YELLOWENERGY},
	{"holyarea",		MAGIC_EFFECT_HOLYAREA},
	{"bigplants",		MAGIC_EFFECT_BIGPLANTS},
	{"cake",		MAGIC_EFFECT_CAKE},
	{"giantice",		MAGIC_EFFECT_GIANTICE},
	{"watersplash",		MAGIC_EFFECT_WATERSPLASH},
	{"plantattack",		MAGIC_EFFECT_PLANTATTACK},
	{"tutorialarrow",	MAGIC_EFFECT_TUTORIALARROW},
	{"tutorialsquare",	MAGIC_EFFECT_TUTORIALSQUARE},
	{"mirrorhorizontal",	MAGIC_EFFECT_MIRRORHORIZONTAL},
	{"mirrorvertical",	MAGIC_EFFECT_MIRRORVERTICAL},
	{"skullhorizontal",	MAGIC_EFFECT_SKULLHORIZONTAL},
	{"skullvertical",	MAGIC_EFFECT_SKULLVERTICAL},
	{"assassin",		MAGIC_EFFECT_ASSASSIN},
	{"stepshorizontal",	MAGIC_EFFECT_STEPSHORIZONTAL},
	{"bloodysteps",		MAGIC_EFFECT_BLOODYSTEPS},
	{"stepsvertical",	MAGIC_EFFECT_STEPSVERTICAL},
	{"yalaharighost",	MAGIC_EFFECT_YALAHARIGHOST},
	{"bats",		MAGIC_EFFECT_BATS},
	{"smoke",		MAGIC_EFFECT_SMOKE},
	{"insects",		MAGIC_EFFECT_INSECTS},
		{"efeito70",	MAGIC_EFFECT_EFEITO70},
    {"efeito71",        MAGIC_EFFECT_EFEITO71},
    {"efeito72",        MAGIC_EFFECT_EFEITO72},
    {"efeito73",        MAGIC_EFFECT_EFEITO73},
    {"efeito74",        MAGIC_EFFECT_EFEITO74},
    {"efeito75",        MAGIC_EFFECT_EFEITO75},
    {"efeito76",        MAGIC_EFFECT_EFEITO76},
    {"efeito77",        MAGIC_EFFECT_EFEITO77},
    {"efeito78",        MAGIC_EFFECT_EFEITO78},
    {"efeito79",        MAGIC_EFFECT_EFEITO79},
    {"efeito80",        MAGIC_EFFECT_EFEITO80},
    {"efeito81",        MAGIC_EFFECT_EFEITO81},
    {"efeito82",        MAGIC_EFFECT_EFEITO82},
    {"efeito83",        MAGIC_EFFECT_EFEITO83},
    {"efeito84",        MAGIC_EFFECT_EFEITO84},
    {"efeito85",        MAGIC_EFFECT_EFEITO85},
    {"efeito86",        MAGIC_EFFECT_EFEITO86},
    {"efeito87",        MAGIC_EFFECT_EFEITO87},
    {"efeito88",        MAGIC_EFFECT_EFEITO88},
    {"efeito89",        MAGIC_EFFECT_EFEITO89},
    {"efeito90",        MAGIC_EFFECT_EFEITO90},
    {"efeito91",        MAGIC_EFFECT_EFEITO91},
    {"efeito92",        MAGIC_EFFECT_EFEITO92},
    {"efeito93",        MAGIC_EFFECT_EFEITO93},
    {"efeito94",        MAGIC_EFFECT_EFEITO94},
    {"efeito95",        MAGIC_EFFECT_EFEITO95},
    {"efeito96",        MAGIC_EFFECT_EFEITO96},
    {"efeito97",        MAGIC_EFFECT_EFEITO97},
    {"efeito98",        MAGIC_EFFECT_EFEITO98},
    {"efeito99",        MAGIC_EFFECT_EFEITO99},
    {"efeito100",        MAGIC_EFFECT_EFEITO100},
    {"efeito101",        MAGIC_EFFECT_EFEITO101},
    {"efeito102",        MAGIC_EFFECT_EFEITO102},
    {"efeito103",        MAGIC_EFFECT_EFEITO103},
    {"efeito104",        MAGIC_EFFECT_EFEITO104},
    {"efeito105",        MAGIC_EFFECT_EFEITO105},
    {"efeito106",        MAGIC_EFFECT_EFEITO106},
    {"efeito107",        MAGIC_EFFECT_EFEITO107},
    {"efeito108",        MAGIC_EFFECT_EFEITO108},
    {"efeito109",        MAGIC_EFFECT_EFEITO109},
    {"efeito110",        MAGIC_EFFECT_EFEITO110},
    {"efeito111",        MAGIC_EFFECT_EFEITO111},
    {"efeito112",        MAGIC_EFFECT_EFEITO112},
    {"efeito113",        MAGIC_EFFECT_EFEITO113},
    {"efeito114",        MAGIC_EFFECT_EFEITO114},
    {"efeito115",        MAGIC_EFFECT_EFEITO115},
    {"efeito116",        MAGIC_EFFECT_EFEITO116},
    {"efeito117",        MAGIC_EFFECT_EFEITO117},
    {"efeito118",        MAGIC_EFFECT_EFEITO118},
    {"efeito119",        MAGIC_EFFECT_EFEITO119},
    {"efeito120",        MAGIC_EFFECT_EFEITO120},
    {"efeito121",        MAGIC_EFFECT_EFEITO121},
    {"efeito122",        MAGIC_EFFECT_EFEITO122},
    {"efeito123",        MAGIC_EFFECT_EFEITO123},
    {"efeito124",        MAGIC_EFFECT_EFEITO124},
    {"efeito125",        MAGIC_EFFECT_EFEITO125},
    {"efeito126",        MAGIC_EFFECT_EFEITO126},
    {"efeito127",        MAGIC_EFFECT_EFEITO127},
    {"efeito128",        MAGIC_EFFECT_EFEITO128},
    {"efeito129",        MAGIC_EFFECT_EFEITO129},
    {"efeito130",        MAGIC_EFFECT_EFEITO130},
    {"efeito131",        MAGIC_EFFECT_EFEITO131},
    {"efeito132",        MAGIC_EFFECT_EFEITO132},
    {"efeito133",        MAGIC_EFFECT_EFEITO133},
    {"efeito134",        MAGIC_EFFECT_EFEITO134},
    {"efeito135",        MAGIC_EFFECT_EFEITO135},
    {"efeito136",        MAGIC_EFFECT_EFEITO136},
    {"efeito137",        MAGIC_EFFECT_EFEITO137},
    {"efeito138",        MAGIC_EFFECT_EFEITO138},
    {"efeito139",        MAGIC_EFFECT_EFEITO139},
    {"efeito140",        MAGIC_EFFECT_EFEITO140},
    {"efeito141",        MAGIC_EFFECT_EFEITO141},
    {"efeito142",        MAGIC_EFFECT_EFEITO142},
    {"efeito143",        MAGIC_EFFECT_EFEITO143},
    {"efeito144",        MAGIC_EFFECT_EFEITO144},
    {"efeito145",        MAGIC_EFFECT_EFEITO145},
    {"efeito146",        MAGIC_EFFECT_EFEITO146},
    {"efeito147",        MAGIC_EFFECT_EFEITO147},
    {"efeito148",        MAGIC_EFFECT_EFEITO148},
    {"efeito149",        MAGIC_EFFECT_EFEITO149},
    {"efeito150",        MAGIC_EFFECT_EFEITO150},
    {"efeito151",        MAGIC_EFFECT_EFEITO151},
    {"efeito152",        MAGIC_EFFECT_EFEITO152},
    {"efeito153",        MAGIC_EFFECT_EFEITO153},
    {"efeito154",        MAGIC_EFFECT_EFEITO154},
    {"efeito155",        MAGIC_EFFECT_EFEITO155},
    {"efeito156",        MAGIC_EFFECT_EFEITO156},
    {"efeito157",        MAGIC_EFFECT_EFEITO157},
    {"efeito158",        MAGIC_EFFECT_EFEITO158},
    {"efeito159",        MAGIC_EFFECT_EFEITO159},
    {"efeito160",        MAGIC_EFFECT_EFEITO160},
    {"efeito161",        MAGIC_EFFECT_EFEITO161},
    {"efeito162",        MAGIC_EFFECT_EFEITO162},
    {"efeito163",        MAGIC_EFFECT_EFEITO163},
    {"efeito164",        MAGIC_EFFECT_EFEITO164},
    {"efeito165",        MAGIC_EFFECT_EFEITO165},
    {"efeito166",        MAGIC_EFFECT_EFEITO166},
    {"efeito167",        MAGIC_EFFECT_EFEITO167},
    {"efeito168",        MAGIC_EFFECT_EFEITO168},
    {"efeito169",        MAGIC_EFFECT_EFEITO169},
    {"efeito170",        MAGIC_EFFECT_EFEITO170},
    {"efeito171",        MAGIC_EFFECT_EFEITO171},
    {"efeito172",        MAGIC_EFFECT_EFEITO172},
    {"efeito173",        MAGIC_EFFECT_EFEITO173},
    {"efeito174",        MAGIC_EFFECT_EFEITO174},
    {"efeito175",        MAGIC_EFFECT_EFEITO175},
    {"efeito176",        MAGIC_EFFECT_EFEITO176},
    {"efeito177",        MAGIC_EFFECT_EFEITO177},
    {"efeito178",        MAGIC_EFFECT_EFEITO178},
    {"efeito179",        MAGIC_EFFECT_EFEITO179},
    {"efeito180",        MAGIC_EFFECT_EFEITO180},
    {"efeito181",        MAGIC_EFFECT_EFEITO181},
    {"efeito182",        MAGIC_EFFECT_EFEITO182},
    {"efeito183",        MAGIC_EFFECT_EFEITO183},
    {"efeito184",        MAGIC_EFFECT_EFEITO184},
    {"efeito185",        MAGIC_EFFECT_EFEITO185},
    {"efeito186",        MAGIC_EFFECT_EFEITO186},
    {"efeito187",        MAGIC_EFFECT_EFEITO187},
    {"efeito188",        MAGIC_EFFECT_EFEITO188},
    {"efeito189",        MAGIC_EFFECT_EFEITO189},
    {"efeito190",        MAGIC_EFFECT_EFEITO190},
    {"efeito191",        MAGIC_EFFECT_EFEITO191},
    {"efeito192",        MAGIC_EFFECT_EFEITO192},
    {"efeito193",        MAGIC_EFFECT_EFEITO193},
    {"efeito194",        MAGIC_EFFECT_EFEITO194},
    {"efeito195",        MAGIC_EFFECT_EFEITO195},
    {"efeito196",        MAGIC_EFFECT_EFEITO196},
    {"efeito197",        MAGIC_EFFECT_EFEITO197},
    {"efeito198",        MAGIC_EFFECT_EFEITO198},
    {"efeito199",        MAGIC_EFFECT_EFEITO199},
    {"efeito200",        MAGIC_EFFECT_EFEITO200},
    {"efeito201",        MAGIC_EFFECT_EFEITO201},
    {"efeito202",        MAGIC_EFFECT_EFEITO202},
    {"efeito203",        MAGIC_EFFECT_EFEITO203},
    {"efeito204",        MAGIC_EFFECT_EFEITO204},
    {"efeito205",        MAGIC_EFFECT_EFEITO205},
    {"efeito206",        MAGIC_EFFECT_EFEITO206},
    {"efeito207",        MAGIC_EFFECT_EFEITO207},
    {"efeito208",        MAGIC_EFFECT_EFEITO208},
    {"efeito209",        MAGIC_EFFECT_EFEITO209},
    {"efeito210",        MAGIC_EFFECT_EFEITO210},
    {"efeito211",        MAGIC_EFFECT_EFEITO211},
    {"efeito212",        MAGIC_EFFECT_EFEITO212},
    {"efeito213",        MAGIC_EFFECT_EFEITO213},
    {"efeito214",        MAGIC_EFFECT_EFEITO214},
    {"efeito215",        MAGIC_EFFECT_EFEITO215},
    {"efeito216",        MAGIC_EFFECT_EFEITO216},
    {"efeito217",        MAGIC_EFFECT_EFEITO217},
    {"efeito218",        MAGIC_EFFECT_EFEITO218},
    {"efeito219",        MAGIC_EFFECT_EFEITO219},
    {"efeito220",        MAGIC_EFFECT_EFEITO220},
    {"efeito221",        MAGIC_EFFECT_EFEITO221},
    {"efeito222",        MAGIC_EFFECT_EFEITO222},
    {"efeito223",        MAGIC_EFFECT_EFEITO223},
    {"efeito224",        MAGIC_EFFECT_EFEITO224},
    {"efeito225",        MAGIC_EFFECT_EFEITO225},
    {"efeito226",        MAGIC_EFFECT_EFEITO226},
    {"efeito227",        MAGIC_EFFECT_EFEITO227},
    {"efeito228",        MAGIC_EFFECT_EFEITO228},
    {"efeito229",        MAGIC_EFFECT_EFEITO229},
    {"efeito230",        MAGIC_EFFECT_EFEITO230},
    {"efeito231",        MAGIC_EFFECT_EFEITO231},
    {"efeito232",        MAGIC_EFFECT_EFEITO232},
    {"efeito233",        MAGIC_EFFECT_EFEITO233},
    {"efeito234",        MAGIC_EFFECT_EFEITO234},
    {"efeito235",        MAGIC_EFFECT_EFEITO235},
    {"efeito236",        MAGIC_EFFECT_EFEITO236},
    {"efeito237",        MAGIC_EFFECT_EFEITO237},
    {"efeito238",        MAGIC_EFFECT_EFEITO238},
    {"efeito239",        MAGIC_EFFECT_EFEITO239},
    {"efeito240",        MAGIC_EFFECT_EFEITO240},
    {"efeito241",        MAGIC_EFFECT_EFEITO241},
    {"efeito242",        MAGIC_EFFECT_EFEITO242},
    {"efeito243",        MAGIC_EFFECT_EFEITO243},
    {"efeito244",        MAGIC_EFFECT_EFEITO244},
    {"efeito245",        MAGIC_EFFECT_EFEITO245},
    {"efeito246",        MAGIC_EFFECT_EFEITO246},
    {"efeito247",        MAGIC_EFFECT_EFEITO247},
    {"efeito248",        MAGIC_EFFECT_EFEITO248},
    {"efeito249",        MAGIC_EFFECT_EFEITO249},
    {"efeito250",        MAGIC_EFFECT_EFEITO250},
    {"efeito251",        MAGIC_EFFECT_EFEITO251},
    {"efeito252",        MAGIC_EFFECT_EFEITO252},
    {"efeito253",        MAGIC_EFFECT_EFEITO253},
    {"efeito254",        MAGIC_EFFECT_EFEITO254},
    {"efeito255",        MAGIC_EFFECT_EFEITO255},
    {"semfim",        MAGIC_EFFECT_SEMFIM}
	
};

ShootTypeNames shootTypeNames[] =
{
	{"spear",		SHOOT_EFFECT_SPEAR},
	{"bolt",		SHOOT_EFFECT_BOLT},
	{"arrow",		SHOOT_EFFECT_ARROW},
	{"fire",		SHOOT_EFFECT_FIRE},
	{"energy",		SHOOT_EFFECT_ENERGY},
	{"poisonarrow",		SHOOT_EFFECT_POISONARROW},
	{"burstarrow",		SHOOT_EFFECT_BURSTARROW},
	{"throwingstar",	SHOOT_EFFECT_THROWINGSTAR},
	{"throwingknife",	SHOOT_EFFECT_THROWINGKNIFE},
	{"smallstone",		SHOOT_EFFECT_SMALLSTONE},
	{"death",		SHOOT_EFFECT_DEATH},
	{"largerock",		SHOOT_EFFECT_LARGEROCK},
	{"snowball",		SHOOT_EFFECT_SNOWBALL},
	{"powerbolt",		SHOOT_EFFECT_POWERBOLT},
	{"poison",		SHOOT_EFFECT_POISONFIELD},
	{"infernalbolt",	SHOOT_EFFECT_INFERNALBOLT},
	{"huntingspear",	SHOOT_EFFECT_HUNTINGSPEAR},
	{"enchantedspear",	SHOOT_EFFECT_ENCHANTEDSPEAR},
	{"redstar",		SHOOT_EFFECT_REDSTAR},
	{"greenstar",		SHOOT_EFFECT_GREENSTAR},
	{"royalspear",		SHOOT_EFFECT_ROYALSPEAR},
	{"sniperarrow",		SHOOT_EFFECT_SNIPERARROW},
	{"onyxarrow",		SHOOT_EFFECT_ONYXARROW},
	{"piercingbolt",	SHOOT_EFFECT_PIERCINGBOLT},
	{"whirlwindsword",	SHOOT_EFFECT_WHIRLWINDSWORD},
	{"whirlwindaxe",	SHOOT_EFFECT_WHIRLWINDAXE},
	{"whirlwindclub",	SHOOT_EFFECT_WHIRLWINDCLUB},
	{"etherealspear",	SHOOT_EFFECT_ETHEREALSPEAR},
	{"ice",			SHOOT_EFFECT_ICE},
	{"earth",		SHOOT_EFFECT_EARTH},
	{"holy",		SHOOT_EFFECT_HOLY},
	{"suddendeath",		SHOOT_EFFECT_SUDDENDEATH},
	{"flasharrow",		SHOOT_EFFECT_FLASHARROW},
	{"flammingarrow",	SHOOT_EFFECT_FLAMMINGARROW},
	{"flamingarrow",	SHOOT_EFFECT_FLAMMINGARROW},
	{"shiverarrow",		SHOOT_EFFECT_SHIVERARROW},
	{"energyball",		SHOOT_EFFECT_ENERGYBALL},
	{"smallice",		SHOOT_EFFECT_SMALLICE},
	{"smallholy",		SHOOT_EFFECT_SMALLHOLY},
	{"smallearth",		SHOOT_EFFECT_SMALLEARTH},
	{"eartharrow",		SHOOT_EFFECT_EARTHARROW},
	{"explosion",		SHOOT_EFFECT_EXPLOSION},
	{"cake",		SHOOT_EFFECT_CAKE},
	{"semfim",		SHOOT_EFFECT_SEMFIM}
};

CombatTypeNames combatTypeNames[] =
{
	{"physical",		COMBAT_PHYSICALDAMAGE},
	{"energy",		COMBAT_ENERGYDAMAGE},
	{"earth",		COMBAT_EARTHDAMAGE},
	{"fire",		COMBAT_FIREDAMAGE},
	{"undefined",		COMBAT_UNDEFINEDDAMAGE},
	{"lifedrain",		COMBAT_LIFEDRAIN},
	{"life drain",		COMBAT_LIFEDRAIN},
	{"manadrain",		COMBAT_MANADRAIN},
	{"mana drain",		COMBAT_MANADRAIN},
	{"healing",		COMBAT_HEALING},
	{"drown",		COMBAT_DROWNDAMAGE},
	{"ice",			COMBAT_ICEDAMAGE},
	{"holy",		COMBAT_HOLYDAMAGE},
	{"death",		COMBAT_DEATHDAMAGE}
};

AmmoTypeNames ammoTypeNames[] =
{
	{"spear",		AMMO_SPEAR},
	{"arrow",		AMMO_ARROW},
	{"poisonarrow",		AMMO_ARROW},
	{"burstarrow",		AMMO_ARROW},
	{"bolt",		AMMO_BOLT},
	{"powerbolt",		AMMO_BOLT},
	{"smallstone",		AMMO_STONE},
	{"largerock",		AMMO_STONE},
	{"throwingstar",	AMMO_THROWINGSTAR},
	{"throwingknife",	AMMO_THROWINGKNIFE},
	{"snowball",		AMMO_SNOWBALL},
	{"huntingspear",	AMMO_SPEAR},
	{"royalspear",		AMMO_SPEAR},
	{"enchantedspear",	AMMO_SPEAR},
	{"sniperarrow",		AMMO_ARROW},
	{"onyxarrow",		AMMO_ARROW},
	{"piercingbolt",	AMMO_BOLT},
	{"infernalbolt",	AMMO_BOLT},
	{"flasharrow",		AMMO_ARROW},
	{"flammingarrow",	AMMO_ARROW},
	{"flamingarrow",	AMMO_ARROW},
	{"shiverarrow",		AMMO_ARROW},
	{"eartharrow",		AMMO_ARROW},
	{"etherealspear",	AMMO_SPEAR}
};

AmmoActionNames ammoActionNames[] =
{
	{"move",		AMMOACTION_MOVE},
	{"moveback",		AMMOACTION_MOVEBACK},
	{"move back",		AMMOACTION_MOVEBACK},
	{"removecharge",	AMMOACTION_REMOVECHARGE},
	{"remove charge",	AMMOACTION_REMOVECHARGE},
	{"removecount",		AMMOACTION_REMOVECOUNT},
	{"remove count",	AMMOACTION_REMOVECOUNT}
};

FluidTypeNames fluidTypeNames[] =
{
	{"none",		FLUID_NONE},
	{"water",		FLUID_WATER},
	{"blood",		FLUID_BLOOD},
	{"beer",		FLUID_BEER},
	{"slime",		FLUID_SLIME},
	{"lemonade",		FLUID_LEMONADE},
	{"milk",		FLUID_MILK},
	{"mana",		FLUID_MANA},
	{"life",		FLUID_LIFE},
	{"oil",			FLUID_OIL},
	{"urine",		FLUID_URINE},
	{"coconutmilk",		FLUID_COCONUTMILK},
	{"coconut milk",	FLUID_COCONUTMILK},
	{"wine",		FLUID_WINE},
	{"mud",			FLUID_MUD},
	{"fruitjuice",		FLUID_FRUITJUICE},
	{"fruit juice",		FLUID_FRUITJUICE},
	{"lava",		FLUID_LAVA},
	{"rum",			FLUID_RUM},
	{"swamp",		FLUID_SWAMP},
	{"tea",			FLUID_TEA},
	{"mead",		FLUID_MEAD}
};

SkillIdNames skillIdNames[] =
{
	{"fist",		SKILL_FIST},
	{"club",		SKILL_CLUB},
	{"sword",		SKILL_SWORD},
	{"axe",			SKILL_AXE},
	{"distance",		SKILL_DIST},
	{"dist",		SKILL_DIST},
	{"shielding",		SKILL_SHIELD},
	{"shield",		SKILL_SHIELD},
	{"fishing",		SKILL_FISH},
	{"fish",		SKILL_FISH},
	{"level",		SKILL__LEVEL},
	{"magiclevel",		SKILL__MAGLEVEL},
	{"magic level",		SKILL__MAGLEVEL}
};

MagicEffect_t getMagicEffect(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(magicEffectNames) / sizeof(MagicEffectNames); ++i)
	{
		if(!strcasecmp(strValue.c_str(), magicEffectNames[i].name))
			return magicEffectNames[i].magicEffect;
	}

	return MAGIC_EFFECT_UNKNOWN;
}

ShootEffect_t getShootType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(shootTypeNames) / sizeof(ShootTypeNames); ++i)
	{
		if(!strcasecmp(strValue.c_str(), shootTypeNames[i].name))
			return shootTypeNames[i].shootType;
	}

	return SHOOT_EFFECT_UNKNOWN;
}

CombatType_t getCombatType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(combatTypeNames) / sizeof(CombatTypeNames); ++i)
	{
		if(!strcasecmp(strValue.c_str(), combatTypeNames[i].name))
			return combatTypeNames[i].combatType;
	}

	return COMBAT_NONE;
}

Ammo_t getAmmoType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(ammoTypeNames) / sizeof(AmmoTypeNames); ++i)
	{
		if(!strcasecmp(strValue.c_str(), ammoTypeNames[i].name))
			return ammoTypeNames[i].ammoType;
	}

	return AMMO_NONE;
}

AmmoAction_t getAmmoAction(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(ammoActionNames) / sizeof(AmmoActionNames); ++i)
	{
		if(!strcasecmp(strValue.c_str(), ammoActionNames[i].name))
			return ammoActionNames[i].ammoAction;
	}

	return AMMOACTION_NONE;
}

FluidTypes_t getFluidType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(fluidTypeNames) / sizeof(FluidTypeNames); ++i)
	{
		if(!strcasecmp(strValue.c_str(), fluidTypeNames[i].name))
			return fluidTypeNames[i].fluidType;
	}

	return FLUID_NONE;
}

skills_t getSkillId(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(skillIdNames) / sizeof(SkillIdNames); ++i)
	{
		if(!strcasecmp(strValue.c_str(), skillIdNames[i].name))
			return skillIdNames[i].skillId;
	}

	return SKILL_FIST;
}

std::string getCombatName(CombatType_t combatType)
{
	switch(combatType)
	{
		case COMBAT_PHYSICALDAMAGE:
			return "physical";
		case COMBAT_ENERGYDAMAGE:
			return "energy";
		case COMBAT_EARTHDAMAGE:
			return "earth";
		case COMBAT_FIREDAMAGE:
			return "fire";
		case COMBAT_UNDEFINEDDAMAGE:
			return "undefined";
		case COMBAT_LIFEDRAIN:
			return "life drain";
		case COMBAT_MANADRAIN:
			return "mana drain";
		case COMBAT_HEALING:
			return "healing";
		case COMBAT_DROWNDAMAGE:
			return "drown";
		case COMBAT_ICEDAMAGE:
			return "ice";
		case COMBAT_HOLYDAMAGE:
			return "holy";
		case COMBAT_DEATHDAMAGE:
			return "death";
		default:
			break;
	}

	return "unknown";
}

std::string getSkillName(uint16_t skillId, bool suffix/* = true*/)
{
	switch(skillId)
	{
		case SKILL_FIST:
		{
			std::string tmp = "taijutsu";
			if(suffix)
				tmp += "";

			return tmp;
		}
		case SKILL_CLUB:
		{
			std::string tmp = "ninjutsu";
			if(suffix)
				tmp += "";

			return tmp;
		}
		case SKILL_SWORD:
		{
			std::string tmp = "weapon";
			if(suffix)
				tmp += "";

			return tmp;
		}
		case SKILL_AXE:
		{
			std::string tmp = "doujutsu";
			if(suffix)
				tmp += "";

			return tmp;
		}
		case SKILL_DIST:
		{
			std::string tmp = "distance";
			if(suffix)
				tmp += " fighting";

			return tmp;
		}
		case SKILL_SHIELD:
			return "defense";
		case SKILL_FISH:
			return "attackspeed";
		case SKILL__MAGLEVEL:
			return "chakra level";
		case SKILL__LEVEL:
			return "level";
		default:
			break;
	}

	return "unknown";
}

std::string getReason(int32_t reasonId)
{
	switch(reasonId)
	{
		case 0:
			return "Offensive Name";
		case 1:
			return "Invalid Name Format";
		case 2:
			return "Unsuitable Name";
		case 3:
			return "Name Inciting Rule Violation";
		case 4:
			return "Offensive Statement";
		case 5:
			return "Spamming";
		case 6:
			return "Illegal Advertising";
		case 7:
			return "Off-Topic Public Statement";
		case 8:
			return "Non-English Public Statement";
		case 9:
			return "Inciting Rule Violation";
		case 10:
			return "Bug Abuse";
		case 11:
			return "Game Weakness Abuse";
		case 12:
			return "Using Unofficial Software to Play";
		case 13:
			return "Hacking";
		case 14:
			return "Multi-Clienting";
		case 15:
			return "Account Trading or Sharing";
		case 16:
			return "Threatening Gamemaster";
		case 17:
			return "Pretending to Have Influence on Rule Enforcement";
		case 18:
			return "False Report to Gamemaster";
		case 19:
			return "Destructive Behaviour";
		case 20:
			return "Excessive Unjustified Player Killing";
		default:
			break;
	}

	return "Unknown Reason";
}

std::string getAction(ViolationAction_t actionId, bool ipBanishment)
{
	std::string action = "Unknown";
	switch(actionId)
	{
		case ACTION_NOTATION:
			action = "Notation";
			break;
		case ACTION_NAMEREPORT:
			action = "Name Report";
			break;
		case ACTION_BANISHMENT:
			action = "Banishment";
			break;
		case ACTION_BANREPORT:
			action = "Name Report + Banishment";
			break;
		case ACTION_BANFINAL:
			action = "Banishment + Final Warning";
			break;
		case ACTION_BANREPORTFINAL:
			action = "Name Report + Banishment + Final Warning";
			break;
		case ACTION_STATEMENT:
			action = "Statement Report";
			break;
		//internal use
		case ACTION_DELETION:
			action = "Deletion";
			break;
		case ACTION_NAMELOCK:
			action = "Name Lock";
			break;
		case ACTION_BANLOCK:
			action = "Name Lock + Banishment";
			break;
		case ACTION_BANLOCKFINAL:
			action = "Name Lock + Banishment + Final Warning";
			break;
		default:
			break;
	}

	if(ipBanishment)
		action += " + IP Banishment";

	return action;
}

std::string parseVocationString(StringVec vocStringVec)
{
	std::string str = "";
	if(!vocStringVec.empty())
	{
		for(StringVec::iterator it = vocStringVec.begin(); it != vocStringVec.end(); ++it)
		{
			if((*it) != vocStringVec.front())
			{
				if((*it) != vocStringVec.back())
					str += ", ";
				else
					str += " and ";
			}

			str += (*it);
			str += "s";
		}
	}

	return str;
}

bool parseVocationNode(xmlNodePtr vocationNode, VocationMap& vocationMap, StringVec& vocStringVec, std::string& errorStr)
{
	if(xmlStrcmp(vocationNode->name,(const xmlChar*)"vocation"))
		return true;

	int32_t vocationId = -1;
	std::string strValue, tmpStrValue;
	if(readXMLString(vocationNode, "name", strValue))
	{
		vocationId = Vocations::getInstance()->getVocationId(strValue);
		if(vocationId != -1)
		{
			vocationMap[vocationId] = true;
			int32_t promotedVocation = Vocations::getInstance()->getPromotedVocation(vocationId);
			if(promotedVocation != -1)
				vocationMap[promotedVocation] = true;
		}
		else
		{
			errorStr = "Wrong vocation name: " + strValue;
			return false;
		}
	}
	else if(readXMLString(vocationNode, "id", strValue))
	{
		IntegerVec intVector;
		if(!parseIntegerVec(strValue, intVector))
		{
			errorStr = "Invalid vocation id - '" + strValue + "'";
			return false;
		}

		size_t size = intVector.size();
		for(size_t i = 0; i < size; ++i)
		{
			Vocation* vocation = Vocations::getInstance()->getVocation(intVector[i]);
			if(vocation && vocation->getName() != "")
			{
				vocationId = vocation->getId();
				strValue = vocation->getName();

				vocationMap[vocationId] = true;
				int32_t promotedVocation = Vocations::getInstance()->getPromotedVocation(vocationId);
				if(promotedVocation != -1)
					vocationMap[promotedVocation] = true;
			}
			else
			{
				std::stringstream ss;
				ss << "Wrong vocation id: " << intVector[i];

				errorStr = ss.str();
				return false;
			}
		}
	}

	if(vocationId != -1 && (!readXMLString(vocationNode, "showInDescription", tmpStrValue) || booleanString(tmpStrValue)))
		vocStringVec.push_back(asLowerCaseString(strValue));

	return true;
}

bool parseIntegerVec(std::string str, IntegerVec& intVector)
{
	StringVec strVector = explodeString(str, ";");
	IntegerVec tmpIntVector;
	for(StringVec::iterator it = strVector.begin(); it != strVector.end(); ++it)
	{
		tmpIntVector = vectorAtoi(explodeString((*it), "-"));
		if(!tmpIntVector[0] && it->substr(0, 1) != "0")
			continue;

		intVector.push_back(tmpIntVector[0]);
		if(tmpIntVector.size() > 1)
		{
			while(tmpIntVector[0] < tmpIntVector[1])
				intVector.push_back(++tmpIntVector[0]);
		}
	}

	return true;
}

bool fileExists(const char* filename)
{
	FILE* f = fopen(filename, "rb");
	if(!f)
		return false;

	fclose(f);
	return true;
}

uint32_t adlerChecksum(uint8_t* data, size_t length)
{
	if(length > NETWORK_MAX_SIZE || !length)
		return 0;

	const uint16_t adler = 65521;
	uint32_t a = 1, b = 0;
	while(length > 0)
	{
		size_t tmp = length > 5552 ? 5552 : length;
		length -= tmp;
		do
		{
			a += *data++;
			b += a;
		}
		while(--tmp);
		a %= adler;
		b %= adler;
	}
	
	return (b << 16) | a;
}

std::string getFilePath(FileType_t type, std::string name/* = ""*/)
{
	#ifdef __FILESYSTEM_HIERARCHY_STANDARD__
	std::string path = "/var/lib/tfs/";
	#endif
	std::string path = g_config.getString(ConfigManager::DATA_DIRECTORY);
	switch(type)
	{
		case FILE_TYPE_OTHER:
			path += name;
			break;
		case FILE_TYPE_XML:
			path += "XML/" + name;
			break;
		case FILE_TYPE_LOG:
			#ifndef __FILESYSTEM_HIERARCHY_STANDARD__
			path = g_config.getString(ConfigManager::LOGS_DIRECTORY) + name;
			#else
			path = "/var/log/tfs/" + name;
			#endif
			break;
		case FILE_TYPE_MOD:
		{
			#ifndef __FILESYSTEM_HIERARCHY_STANDARD__
			path = "mods/" + name;
			#else
			path = "/usr/share/tfs/" + name;
			#endif
			break;
		}
		case FILE_TYPE_CONFIG:
		{
			#if defined(__HOMEDIR_CONF__)
			if(fileExists("~/.tfs/" + name))
				path = "~/.tfs/" + name;
			else
			#endif
			#if defined(__FILESYSTEM_HIERARCHY_STANDARD__)
				path = "/etc/tfs/" + name;
			#else
				path = name;
			#endif
			break;
		}
		default:
			std::clog << "> ERROR: Wrong file type!" << std::endl;
			break;
	}
	return path;
}
