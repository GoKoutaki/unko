#include <stdio.h>
#include <string>
#include <WCHAR.H>
#include <vector>
#include <algorithm>

using namespace std;


wchar_t  cut[8][256];
wchar_t P[] = L"\\textbackslash ";

// divide text by camma
int cutbuf(wchar_t  *buf) {

	memset(&cut[0][0],0,8*256*2);

	int n = 0;
	int l = 0;
	int len = wcslen(buf);
	for (int c = 0; c < len;c++) {


		if (buf[c] == L',') {
			l = 0;
			n++;
		} else {
			cut[n][l] = buf[c];
			l++;
		}
	}

	return n;
};


struct unko_t {

	wchar_t cut[8][256];
	int n;
	bool operator < (const unko_t & ref) {
		int v;
		int v2;
		if(n > 0)v  = cut[1][1] + n;
		else v = cut[0][1];
		if(ref.n > 0) v2 = ref.cut[1][1] + ref.n;
		else v2 = ref.cut[0][1];

		return v < v2;
	};
};

int main(int argc, char **argv){
	
	FILE *fp=fopen(argv[1],"r");
	FILE *fp_o=fopen("./kanji.tex","w");
	
	wchar_t  buf[1024];
	wchar_t  title[1024];
	wchar_t  author[1024];

	vector<unko_t> libs;

	// header
	fwprintf(fp_o, L"\\documentclass[12pt,a4paper]\{jarticle\}\n");
	fwprintf(fp_o, L"\\usepackage[top=10truemm,bottom=15truemm,left=15truemm,right=10truemm]\{geometry\}\n");
	fwprintf(fp_o, L"\\usepackage\{ascmac\}\n");
	fwprintf(fp_o, L"\\usepackage\{pxrubrica\}\n");
	

	// load title and author
	fgetws(title,  1024, fp);
	fgetws(author, 1024, fp);

	fwprintf(fp_o, L"\\title\{%s\}\n",  title);
	fwprintf(fp_o, L"\\author\{%s\}\n", author);
	fwprintf(fp_o, L"\\date\{\}\n");

	fwprintf(fp_o, L"\\begin\{document\}\n");
	fwprintf(fp_o, L"\\maketitle\n");


	// load data
	while (fgetws(buf, 1024, fp)) {

		int n = cutbuf(buf);

		unko_t t;
		t.n = n;
		memcpy(t.cut, cut, 8 * 256 * 2);
		libs.push_back(t);

		int s = n - 2;
		for (int k = 0; k < s; k++) {
			unko_t t2 = t;
			wchar_t tmp[256];
			memcpy(tmp, t2.cut[1], 256 * 2);
			memcpy(t2.cut[1], t2.cut[2 + k], 256 * 2);
			memcpy(t2.cut[2 + k], tmp, 256 * 2);
			libs.push_back(t2);
		}
	}

	// sort data
	sort(libs.begin(), libs.end());

	// output to file
	for(int k = 0;k < libs.size();k++){

		int n = libs[k].n;

		if (n == 0 && libs[k + 1].n == 0)continue;

		memcpy(cut, libs[k].cut, 8 * 256 * 2);

		// head charactor
		if (n == 0)
		{
			fwprintf(fp_o, L"\\noindent \{\\fontsize\{20pt\}\{0pt\}\\selectfont \\bf\n");
			fwprintf(fp_o, cut[0]);
			fwprintf(fp_o, L"\} \\\\ \n");

			continue;
		}
			
		// print kanji
		fwprintf(fp_o, L"\\noindent\n\\begin\{tabular\}\{lp\{14cm\}\} \{\\fontsize\{60pt\}\{0pt\}\\selectfont\n");
		fwprintf(fp_o, cut[0]);
		fwprintf(fp_o, L"\} & \\vspace\{-1.5cm\}\n");

		for (int i = 1;i < n;i++) {

			int f = 0;
			fwprintf(fp_o, L"\{\\bf ");
			for (int n = 0;n < wcslen(cut[i]);n++) {
				if (cut[i][n] == L':') {
					fwprintf(fp_o, L"\}");
					f = 1;
				} else {
					fwprintf(fp_o, L"%c",cut[i][n]);
				}
			}
			if (f == 0) {
				fwprintf(fp_o, L"\}");
			}
			fwprintf(fp_o, P);
		}
		fwprintf(fp_o, &cut[n][1]);
		fwprintf(fp_o, L"\\end\{tabular\}\n\\\\\n");
	}

	fwprintf(fp_o, L"\\end\{document\}\n");


	fclose(fp);
	fclose(fp_o);
	
}

