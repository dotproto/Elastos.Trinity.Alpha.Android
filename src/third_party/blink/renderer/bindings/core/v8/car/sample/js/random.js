
var random = new Random();

const UINT8_MAX = 255;

const INT16_MIN = -32767 - 1;
const INT16_MAX = 32767;

const UINT16_MAX = 65535;

const INT32_MIN = -2147483647 - 1;
const INT32_MAX = 2147483647;

const UINT32_MAX = 4294967295;

const INT64_MIN = -9223372036854775807 - 1;
const INT64_MAX = 9223372036854775807;

const WORD_SIZE = 64

const UINTPTR_MAX = (() => {
  if (WORD_SIZE === 64)
    return 18446744073709551615;

  if (WORD_SIZE === 32)
    return 4294967295;
})();

let int16 = () => random.integer(INT16_MIN, INT16_MAX);

let int32 = () => random.integer(INT32_MIN, INT32_MAX);

let int64 = () => random.integer(INT64_MIN, INT64_MAX);

let byte_ = () => random.integer(0, UINT8_MAX);

let float_ = () => random.real(-Infinity, Infinity);

let double_ = () => random.real(-Infinity, Infinity);

let char32 = () => random.string(1);

let string = () => random.string(128);

let boolean_ = () => random.bool();

let uint32 = () => random.integer(0, UINT32_MAX);

let emuid = () => {
  mData1 = uint32(),
  mData2 = random.integer(0, UINT16_MAX),
  mData3 = random.integer(0, UINT16_MAX),
  mData4 = [
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX)
  ]
};

let eguid = () => {
  mClsid = emuid(),
  mUunm = string(),
  mCarcode = uint32()
};

let ecode = int32;

let localPtr = () => random.integer(0, UINTPTR_MAX);

let int16s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let i16s = [];

  for (let i = 0; i < length; ++i)
    i16s.push(int16());

  return i16s;
};

let int32s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let i32s = [];

  for (let i = 0; i < length; ++i)
    i32s.push(int32());

  return i32s;
};

let int64s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let i64s = [];

  for (let i = 0; i < length; ++i)
    i64s.push(int64());

  return i64s;
};

let bytes = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let bytes = [];

  for (let i = 0; i < length; ++i)
    bytes.push(byte_());

  return bytes;
};

let floats = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let fs_ = [];

  for (let i = 0; i < length; ++i)
    fs_.push(float_());

  return fs_;
};

let doubles = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let ds = [];

  for (let i = 0; i < length; ++i)
    ds.push(double_());

  return ds;
};

let char32s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let c32s = [];

  for (let i = 0; i < length; ++i)
    c32s.push(char32());

  return c32s;
};

let strings = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let ss = [];

  for (let i = 0; i < length; ++i)
    ss.push(string());

  return ss;
};

let booleans = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let bs = [];

  for (let i = 0; i < length; ++i)
    bs.push(random.bool());

  return bs;
};

let emuids = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let emuids = [];

  for (let i = 0; i < length; ++i)
    emuids.push(emuid());

  return emuids;
};

let eguids = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let eguids = [];

  for (let i = 0; i < length; ++i)
    eguids.push(eguid());

  return eguids;
};

let ecodes = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let ecodes = [];

  for (let i = 0; i < length; ++i)
    ecodes.push(ecode());

  return ecodes;
};

random.int16 = int16;
random.int32 = int32;
random.int64 = int64;
random.byte_ = byte_;
random.float_ = float_;
random.double_ = double_;
random.char32 = char32;
random.string = string;
random.boolean_ = boolean_;
random.uint32 = uint32;
random.emuid = emuid;
random.eguid = eguid;
random.ecode = ecode;
random.localPtr = localPtr;
random.int16s = int16s;
random.int32s = int32s;
random.int64s = int64s;
random.bytes = bytes;
random.floats = floats;
random.doubles = doubles;
random.char32s = char32s;
random.strings = strings;
random.booleans = booleans;
random.emuids = emuids;
random.eguids = eguids;
random.ecodes = ecodes;

