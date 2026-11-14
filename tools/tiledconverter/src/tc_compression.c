#include "tc_stream.h"

int tcDecompRLE(const uint8_t* src, uint8_t* dst)
{
    const uint8_t* srcStart = src;
    uint8_t* dstStart = dst;

    for (int p = 0; p < 2; p++)
    {  // for each pass
        uint8_t* dstOffset = dstStart + p;
        bool readOneByte = tcRead8(&src) == 1;
        while (true)
        {
            uint16_t amount;
            uint16_t compressedFlag;
            bool compressed;
            if (readOneByte)
            {
                amount = tcRead8(&src);
                compressedFlag = 0x80;
            }
            else // Read two bytes
            {
                amount = tcRead16Be(&src);
                compressedFlag = 0x8000;
            }
            if (amount == 0)
            {
                break;
            }

            compressed = (amount & compressedFlag) != 0;
            if (compressed)
            {
                amount ^= compressedFlag;
                uint8_t val = tcRead8(&src);
                while (amount > 0)
                {
                    tcSeek(&dst, dstOffset);
                    tcWrite8(&dst, val);
                    dstOffset += 2;
                    amount--;
                }
            }
            else
            {
                while (amount > 0)
                {
                    uint8_t val = tcRead8(&src);
                    tcSeek(&dst, dstOffset);
                    tcWrite8(&dst, val);
                    dstOffset += 2;
                    amount--;
                }
            }
        }
    }

    // compressed size
    return src - srcStart;
}

/*
int tcCompRLE(ByteStream input, int length, ByteStream output)
{
    // assumes input stream starts at 0
    // assumes output stream has proper position
    int destStart = output.Position;
    byte[] inputArr = input.Data;

    // do two passes for even and odd bytes
    for (int pass = 0; pass < 2; pass++)
    {
        // get counts of consecutive values
        List<byte> values = new List<byte>();
        List<int> counts = new List<int>();

        uint8_t prev = inputArr[pass];
        values.Add(prev);
        int count = 1;

        for (int i = pass + 2; i < length; i += 2)
        {
            uint8_t val = inputArr[i];
            if (val == prev)  // if consecutive values are equal
            {
                count++;
            }
            else  // if consecutive values are different
            {
                values.Add(val);
                counts.Add(count);
                prev = val;
                count = 1;
            }
        }
        counts.Add(count);

        // try each read length (1 or 2)
        byte[,] temp = new byte[2, length];
        int[] pos = { 0, 0 };

        for (int r = 0; r < 2; r++)
        {
            int minRunLen = 3 + r;
            int addVal = 0x80 << (8 * r);
            int maxRunLen = addVal - 1;
            List<byte> unique = new List<byte>();

            temp[r, pos[r]++] = (byte)(r + 1);  // write number of bytes to read

            for (int i = 0; i < values.Count; i++)  // for each value and its count
            {
                count = counts[i];
                if (count >= minRunLen)  // if the value's count is long enough for a run
                {
                    if (unique.Count > 0)  // if the value is preceded by unique values
                    {
                        if (r == 0) { temp[r, pos[r]++] = (byte)unique.Count; }
                        else
                        {
                            temp[r, pos[r]++] = (byte)(unique.Count >> 8);
                            temp[r, pos[r]++] = (byte)unique.Count;
                        }
                        foreach (uint8_t val in unique)
                        {
                            temp[r, pos[r]++] = val;
                        }
                        unique.Clear();
                    }
                    // add run length and value (multiple times if over max run length)
                    while (count > 0)
                    {
                        int currLen = addVal + Math.Min(count, maxRunLen);
                        if (r == 0) { temp[r, pos[r]++] = (byte)currLen; }
                        else
                        {
                            temp[r, pos[r]++] = (byte)(currLen >> 8);
                            temp[r, pos[r]++] = (byte)currLen;
                        }
                        temp[r, pos[r]++] = values[i];
                        count -= maxRunLen;
                    }
                }
                else  // if the value's count is too short for a run
                {
                    if (unique.Count + count > maxRunLen)  // if the total count would be too long for a run
                    {
                        if (r == 0) { temp[r, pos[r]++] = (byte)unique.Count; }
                        else
                        {
                            temp[r, pos[r]++] = (byte)(unique.Count >> 8);
                            temp[r, pos[r]++] = (byte)unique.Count;
                        }
                        foreach (uint8_t val in unique)
                        {
                            temp[r, pos[r]++] = val;
                        }
                        unique.Clear();
                    }
                    for (int j = 0; j < count; j++)
                    {
                        unique.Add(values[i]);
                    }
                }
            }
            // check if there were unique values at the end
            if (unique.Count > 0)
            {
                if (r == 0) { temp[r, pos[r]++] = (byte)unique.Count; }
                else
                {
                    temp[r, pos[r]++] = (byte)(unique.Count >> 8);
                    temp[r, pos[r]++] = (byte)unique.Count;
                }
                foreach (uint8_t val in unique)
                {
                    temp[r, pos[r]++] = val;
                }
                unique.Clear();
            }
            // write ending zero(s)
            temp[r, pos[r]++] = 0;
            if (r == 1) { temp[r, pos[r]++] = 0; }
        }
        // check which was shorter and copy to output
        int index = (pos[0] <= pos[1]) ? 0 : 1;
        int len = pos[index];

        for (int i = 0; i < len; i++)
        {
            output.tcWrite8(temp[index, i]);
        }
    }

    return output.Position - destStart;
}
*/

int tcDecompLZ77(const uint8_t* src, uint8_t* dst)
{
    // get length of decompressed data
    const uint8_t* srcStart = src;
    int remain = tcRead32Le(&src) >> 8;

    // decompress
    while (remain > 0)
    {
        uint8_t cflag = tcRead8(&src);
        for (int i = 0; i < 8; i++)
        {
            // uncompressed
            if ((cflag & 0x80) == 0)
            {
                uint8_t val = tcRead8(&src);
                tcWrite8(&dst, val);
                remain--;
            }
            // compressed
            else
            {
                uint8_t val1 = tcRead8(&src);
                uint8_t val2 = tcRead8(&src);
                int amountToCopy = (val1 >> 4) + 3;
                int window = ((val1 & 0xF) << 8) + val2 + 1;
                remain -= amountToCopy;
                tcOverlappingCopy(&dst, amountToCopy, window);
            }
            if (remain <= 0)
            {
                return src - srcStart;
            }
            cflag <<= 1;
        }
    }
    return src - srcStart;
}

/*
int tcCompLZ77(ByteStream input, int length, ByteStream output)
{
    // preprocess (find all 3 uint8_t runs)
    byte[] inputArr = input.Data;
    Dictionary<int, List<int>> runs = new Dictionary<int, List<int>>();
    for (int i = 0; i < input.Length - 2; i++)
    {
        int triplet = inputArr[i] | (inputArr[i + 1] << 8) | (inputArr[i + 2] << 16);

        List<int> indexes;
        if (runs.TryGetValue(triplet, out indexes))
        {
            indexes.Add(i);
        }
        else
        {
            runs.Add(triplet, new List<int>() { i });
        }
    }

    // assumes input stream starts at 0
    // assumes output stream has proper position
    int maxRunLen = 18;
    int windowSize = 0x1000;
    int source = 0;
    int destStart = output.Position;

    // write start of data
    output.tcWrite8(0x10);
    output.tcWrite8((byte)length);
    output.tcWrite8((byte)(length >> 8));
    output.tcWrite8((byte)(length >> 16));

    while (source < length)
    {
        int flag = output.Position;
        output.tcWrite8(0);

        for (int i = 0; i < 8; i++)
        {
            // get next three bytes
            if (source + 3 > length) { goto Uncompressed; }
            int triplet = inputArr[source] | (inputArr[source + 1] << 8) | (inputArr[source + 2] << 16);

            List<int> indexes;
            if (runs.TryGetValue(triplet, out indexes))
            {
                // find start of indexes to check
                int j = 0;
                while (indexes[j] < source - windowSize)
                {
                    j++;
                    if (j == indexes.Count) { goto Uncompressed; }
                }

                // find longest run
                int longestRunLen = -1;
                int longestRunIndex = -1;

                for (; j < indexes.Count; j++)
                {
                    int index = indexes[j];
                    if (index >= source - 1) { break; }

                    int run = 3;
                    while ((source + run < length) && (inputArr[index + run] == inputArr[source + run]) && (run < maxRunLen))
                    {
                        run++;
                    }

                    if (run > longestRunLen)
                    {
                        longestRunLen = run;
                        longestRunIndex = index;
                    }
                }

                if (longestRunIndex == -1)
                {
                    goto Uncompressed;
                }

                int runOffset = source - longestRunIndex - 1;
                output.tcWrite8((byte)(((longestRunLen - 3) << 4) | (runOffset >> 8)));
                output.tcWrite8((byte)runOffset);
                output.Data[flag] |= (byte)(0x80 >> i);
                source += longestRunLen;
                goto End;
            }

        Uncompressed:
            output.tcWrite8(inputArr[source++]);

        End:
            if (source >= length) { break; }
        }
    }

    return output.Position - destStart;
}
*/
