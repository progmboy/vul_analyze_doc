
#include <stdio.h>
#include <windows.h>

signed int RtlULongAdd(unsigned int a1, int a2, unsigned int *a3)
{
	unsigned int v3; // edx@1
	signed int result; // eax@2

	v3 = a1 + a2;
	if (v3 < a1)
	{
		*a3 = -1;
		result = -1073741675;
	} else
	{
		*a3 = v3;
		result = 0;
	}
	return result;
}

unsigned int SrvOs2FeaListSizeToNt(PUCHAR pOs2Fea)
{
	unsigned int v1; // edi@1
	int Length; // ebx@1
	PUCHAR pBody; // esi@1
	PUCHAR v4; // ebx@1
	int v5; // ecx@3
	int v8; // [sp+10h] [bp-8h]@3
	unsigned int v9; // [sp+14h] [bp-4h]@1

	v1 = 0;
	Length = *(DWORD*)pOs2Fea;
	pBody = pOs2Fea + 4;
	v9 = 0;
	v4 = pOs2Fea + Length;
	while (pBody < v4)
	{
		if (pBody + 4 >= v4
			|| (v5 = *(BYTE *)(pBody + 1) + *(WORD *)(pBody + 2),
				v8 = *(BYTE *)(pBody + 1) + *(WORD *)(pBody + 2),
				v5 + pBody + 5 > v4))
		{
			*(WORD *)pOs2Fea = pBody - pOs2Fea;
			return v1;
		}
		if (RtlULongAdd(v1, (v5 + 0xC) & 0xFFFFFFFC, &v9) < 0)
			return 0;
		v1 = v9;
		pBody += v8 + 5;
	}
	return v1;
}

PUCHAR gpBuffer = NULL;
ULONG guSize = 0;

PUCHAR SrvOs2FeaToNt(PUCHAR pNtFea, PUCHAR pOs2FeaBody)
{
	PUCHAR pBody; // edi@1
	BYTE *pNtBodyStart; // edi@1
	PUCHAR result; // eax@1

	pBody = pNtFea + 8;
	*(BYTE *)(pNtFea + 4) = *(BYTE *)pOs2FeaBody;
	*(BYTE *)(pNtFea + 5) = *(BYTE *)(pOs2FeaBody + 1);
	*(WORD *)(pNtFea + 6) = *(WORD *)(pOs2FeaBody + 2);


	memcpy((void *)(pNtFea + 8), (const void *)(pOs2FeaBody + 4), *(BYTE *)(pOs2FeaBody + 1));
	pNtBodyStart = (BYTE *)(*(BYTE *)(pNtFea + 5) + pBody);
	*pNtBodyStart++ = 0;

	if ((pNtBodyStart + *(WORD *)(pNtFea + 6)) > (gpBuffer + guSize)){
		__debugbreak();
	}
	memcpy(pNtBodyStart, (const void *)(pOs2FeaBody + 5 + *(BYTE *)(pNtFea + 5)), *(WORD *)(pNtFea + 6));
	result = (PUCHAR)((ULONG_PTR)&pNtBodyStart[*(WORD *)(pNtFea + 6) + 3] & 0xFFFFFFFC);
	*(DWORD *)pNtFea = result - pNtFea;
	return result;
}

int main()
{

	FILE* pFile = fopen("d:\\test\\dump.bin", "r+b");
	fseek(pFile, 0, SEEK_END);
	ULONG uSize = (ULONG)ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	PUCHAR pOs2Fea = (PUCHAR)malloc(uSize);
	fread(pOs2Fea, 1, uSize, pFile);
	fclose(pFile);

	ULONG uSaveSize = *(DWORD*)pOs2Fea;
	ULONG uFixSize = SrvOs2FeaListSizeToNt(pOs2Fea);

	PUCHAR pOs2FeaBody;
	PUCHAR  pNtFea = (PUCHAR)malloc(uFixSize);
	PUCHAR v10;
	PUCHAR v14;
	PUCHAR v12;
	PUCHAR v5;
	LONG v11;

	PUCHAR  pNtFeaEnd = pNtFea + uFixSize;

	gpBuffer = pNtFea;
	guSize = uFixSize;

	if (pNtFea)
	{
		pOs2FeaBody = pOs2Fea + 4;
		v10 = pNtFea;
		v14 = pOs2Fea + *(DWORD *)pOs2Fea - 5;
		if (pOs2Fea + 4 > v14)
		{
		LABEL_13:
			if (pOs2FeaBody == pOs2Fea + *(DWORD *)pOs2Fea)
			{
				*(DWORD *)v10 = 0;
				return 0;
			}
			v11 = 0xC0000001;
			//*a4 = v5 - pOs2Fea;
		} else{
			while (!(*(BYTE *)pOs2FeaBody & 0x7F))
			{
				v12 = pNtFea;
				v5 = pOs2FeaBody;
				pNtFea = SrvOs2FeaToNt(pNtFea, pOs2FeaBody);
				pOs2FeaBody += *(BYTE *)(pOs2FeaBody + 1) + *(WORD *)(pOs2FeaBody + 2) + 5;
				if (pOs2FeaBody > v14)
				{
					v10 = v12;
					goto LABEL_13;
				}
			}
			//*a4 = pOs2FeaBody - pOs2Fea;
			v11 = 0xC000000D;
		}
		return v11;
	}

    return 0;
}

