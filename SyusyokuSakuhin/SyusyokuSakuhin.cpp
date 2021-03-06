#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <conio.h>
#pragma comment(lib, "winmm.lib")

using namespace std;

vector<string> split(string& input, char delimiter) {
	istringstream stream(input);
	string field;
	vector<string> result;
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}
	return result;
}

//打鍵データを表示する関数
void resultData(int* resultNumber, int* resultAlphabet, int dakensu, int backspace, int wpm, string date, double time) {
	printf("\n%sの打鍵データ\n", date.c_str());
	printf("1[%d]  2[%d]  3[%d]  4[%d]  5[%d]  6[%d]  7[%d]  8[%d]  9[%d]  0[%d]\n", resultNumber[1], resultNumber[2], resultNumber[3], resultNumber[4], resultNumber[5], resultNumber[6], resultNumber[7], resultNumber[8], resultNumber[9], resultNumber[0]);
	printf("　Q[%d]  W[%d]  E[%d]  R[%d]  T[%d]  Y[%d]  U[%d]  I[%d]  O[%d]  P[%d]\n", resultAlphabet[16], resultAlphabet[22], resultAlphabet[4], resultAlphabet[17], resultAlphabet[19], resultAlphabet[24], resultAlphabet[20], resultAlphabet[8], resultAlphabet[14], resultAlphabet[15]);
	printf("　　A[%d]  S[%d]  D[%d]  F[%d]  G[%d]  H[%d]  J[%d]  K[%d]  L[%d]\n", resultAlphabet[0], resultAlphabet[18], resultAlphabet[3], resultAlphabet[5], resultAlphabet[6], resultAlphabet[7], resultAlphabet[9], resultAlphabet[10], resultAlphabet[11]);
	printf("　　　Z[%d]  X[%d]  C[%d]  V[%d]  B[%d]  N[%d]  M[%d]\n", resultAlphabet[25], resultAlphabet[23], resultAlphabet[2], resultAlphabet[21], resultAlphabet[1], resultAlphabet[13], resultAlphabet[12]);
	if (time >= 60) {
		time = time / 60;
		printf("計測時間：%.3lf分\n", time);
	}
	else {
		printf("計測時間：%.3lf秒\n", time);
	}
	printf("打鍵数：%d\n", dakensu);
	printf("ミス数：%d\n", backspace);
	printf("WPM：%d\n", wpm);
}


int main() {

	SHORT SPACE, ENTER, BACKSPACE, CTRL, ALT;
	int charCount = 0, backSpaceCount = 0, timeCount = 0, wpm = 0, charCountR = 0, backSpaceCountR = 0, timeCountR = 0, wpmR = 0, sumChar = 0, sumSpace = 0, sumWpm = 0;
	SHORT number[10] = { 0,1,2,3,4,5,6,7,8,9 };
	char alphabet[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
	SHORT az[26];
	int alphabetCount[26] = {}, alphabetCountR[26] = {}, sumAlphabet[26] = {}, numberCount[10] = {}, numberCountR[10] = {}, sumNumber[10] = {}, resultAlphabet[26] = {}, resultNumber[10] = {};
	BOOL flag1 = false, flag2 = false, autoSaveFlag = false;

	DWORD start, end, subStart, subEnd;
	double con = 0, subCon = 0, relayCon = 0, mainTime = 0, mainTimeR = 0, sumTime = 0;
	string fileName;

	cout << "CTRL+ALT+Dで使用可能なコマンド一覧を表示します。" << endl;

	while (1) {
		//仮想キーコードは、WINUSER.H ヘッダファイルで記載されている
		//これがないとCPU使用率が高くなる
		Sleep(1);

		SPACE = GetAsyncKeyState(VK_SPACE);
		ENTER = GetAsyncKeyState(VK_RETURN);
		BACKSPACE = GetAsyncKeyState(VK_BACK);
		CTRL = GetAsyncKeyState(VK_CONTROL);
		ALT = GetAsyncKeyState(VK_MENU);

		if (flag2 == true) {
			//　サブ時間計測開始
			subEnd = timeGetTime();
			subCon = (double)(subEnd - subStart) / 1000;
			//printf("3秒になるまで増える：%lf\n", subCon);
			relayCon = subCon;
		}

		// 入力終了から一定時間経過した時
		if (relayCon >= 2.4) {
			end = timeGetTime();
			con = (double)(end - start) / 1000;
			mainTime += (con - 2.4);
			printf("計測時間：%.3lf\n", mainTime);
			relayCon = 0;
			subCon = 0;
			con = 0;
			flag1 = false;
			flag2 = false;

			// オートセーブ機能
			if (autoSaveFlag) {
				ostringstream oss;

				// 今日の日付を取得
				std::time_t rawtime;
				std::tm timeinfo;
				std::time(&rawtime);
				localtime_s(&timeinfo, &rawtime);
				int nowTime = (1900 + timeinfo.tm_year) * 10000 + (timeinfo.tm_mon + 1) * 100 + timeinfo.tm_mday;
				fileName = "data/" + to_string(nowTime) + ".csv";

				if ((charCount - backSpaceCount) > 0) {
					wpm = (int)(charCount - backSpaceCount) / (mainTime / 60);
				}
				charCountR = 0;
				backSpaceCountR = 0;
				wpmR = 0;
				mainTimeR = 0;

				// 既に同じ名前のファイルがあるか確認
				try {
					ifstream ifs(fileName);
					string line;
					while (getline(ifs, line)) {

						vector<string> strvec = split(line, ',');

						charCountR = stoi(strvec.at(0));
						backSpaceCountR = stoi(strvec.at(1));
						wpmR = stoi(strvec.at(2));
						mainTimeR = stoi(strvec.at(3));

						for (int i = 4; i<30; i++) {
							int alphaI = i - 4;
							alphabetCountR[alphaI] = stoi(strvec.at(i));
						}
						for (int i = 30; i < 40; i++) {
							int numberI = i - 30;
							numberCountR[numberI] = stoi(strvec.at(i));
						}

					}
				}
				catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
				}
				if (charCount < charCountR) {
					sumChar = charCount + charCountR;
					sumSpace = backSpaceCount + backSpaceCountR;
					sumTime = mainTime + mainTimeR;
					if ((sumChar - sumSpace) > 0) {
						sumWpm = (int)(sumChar - sumSpace) / (sumTime / 60);
					}

					for (int i = 0; i < 26; i++) {
						sumAlphabet[i] = alphabetCount[i] + alphabetCountR[i];
					}
					for (int i = 0; i < 10; i++) {
						sumNumber[i] = numberCount[i] + numberCountR[i];
					}

					ofstream ofs(fileName);
					ofs << sumChar << "," << sumSpace << "," << sumWpm << "," << sumTime << ",";

					for (int i = 0; i < 26; i++) {
						ofs << sumAlphabet[i] << ",";
					}
					for (int i = 0; i < 10; i++) {
						ofs << sumNumber[i] << ",";
					}
					ofs << endl;
					charCount = sumChar;
					backSpaceCount = sumSpace;
					wpm = sumWpm;
					mainTime = sumTime;
					for (int i = 0; i < 26; i++) {
						alphabetCount[i] = sumAlphabet[i];
					}
					for (int i = 0; i < 10; i++) {
						numberCount[i] = sumNumber[i];
					}
					cout << "オートセーブ完了" << endl;
					cout << "保存されているデータの打鍵数を現在のデータに挿入しました。" << endl;
				}
				else {
					ofstream ofs(fileName);
					ofs << charCount << "," << backSpaceCount << "," << wpm << "," << mainTime << ",";

					for (int i = 0; i < 26; i++) {
						ofs << alphabetCount[i] << ",";
					}
					for (int i = 0; i < 10; i++) {
						ofs << numberCount[i] << ",";
					}
					ofs << endl;
					cout << "オートセーブ完了" << endl;

				}

			}
		}

		//数字キーを押したときの処理
		//試験的にFor文の中に入れてある
		for (int i = 0; i < 10; i++) {
			char c = '0' + i;
			number[i] = GetAsyncKeyState(c);
			if (number[i] & 0x8000) {
				charCount++;
				numberCount[i]++;
				subStart = timeGetTime();
				flag2 = true;
				if (flag1 == false && flag2 == true) {
					flag1 = true;
					// メイン時間計測開始
					start = timeGetTime();
				}
				while (GetAsyncKeyState(c)) {
					Sleep(1);
				}
			}
		}

		//アルファベットキーを押したときの処理
		//試験的にFor文の中に入れてある→多分このままForの中に入れていく方向になる
		for (int i = 0; i < 26; i++) {
			char c = alphabet[i];
			az[i] = GetAsyncKeyState(c);
			if (az[i] & 0x8000) {
				// 全体の打鍵数
				charCount++;
				// アルファベット事の打鍵数
				alphabetCount[i]++;
				subStart = timeGetTime();
				flag2 = true;
				if (flag1 == false && flag2 == true) {
					flag1 = true;
					// メイン時間計測開始
					start = timeGetTime();
				}
				while (GetAsyncKeyState(c)) {
					Sleep(1);
				}
			}
		}

		if (SPACE & 0x8000) {
			//スペースを押すと空白
			printf(" ");
			while (GetAsyncKeyState(VK_SPACE)) {
				Sleep(1);
			}
		}

		if (ENTER & 0x8000) {
			//エンターを押すと改行
			printf("\n");
			while (GetAsyncKeyState(VK_RETURN)) {
				Sleep(1);
			}
		}

		if (BACKSPACE & 0x8000) {
			//バックスペースを押したとき、ミスとしてカウントする
			backSpaceCount++;
			while (GetAsyncKeyState(VK_BACK)) {
				Sleep(1);
			}
		}

		// CTRL+ALTでコマンドを実行する
		if (CTRL & 0x8000) {
			while (GetAsyncKeyState(VK_CONTROL)) {
				if (GetAsyncKeyState(VK_MENU)) {
					while (GetAsyncKeyState(VK_MENU)) {
						// CTRL+ALTが押されている

						if (GetAsyncKeyState('L')) {
							// CTRL+ALT+L　→　現在のタイプデータを表示
							// WPMはメイン時間（秒）を60で割って分単位にしてから打鍵した文字数で割る
							if ((charCount - backSpaceCount) > 0 && mainTime != 0) {
								wpm = (int)(charCount - backSpaceCount) / (mainTime / 60);
							}

							// 今日の日付を取得
							std::time_t rawtime;
							std::tm timeinfo;
							std::time(&rawtime);
							localtime_s(&timeinfo, &rawtime);
							int nowTime = (1900 + timeinfo.tm_year) * 10000 + (timeinfo.tm_mon + 1) * 100 + timeinfo.tm_mday;

							// 関数用の配列にコピー
							memcpy(resultAlphabet, alphabetCount, sizeof(alphabetCount));
							memcpy(resultNumber, numberCount, sizeof(numberCount));
							// 表示する関数呼び出し
							resultData(resultNumber, resultAlphabet, charCount, backSpaceCount, wpm, to_string(nowTime), mainTime);
							// 初期化
							memset(resultAlphabet, 0, sizeof(resultAlphabet));
							memset(resultNumber, 0, sizeof(resultNumber));

							while (GetAsyncKeyState('L')) {
								Sleep(1);
							}
						}

						// CTRL+ALT+E　→　プログラム終了
						if (GetAsyncKeyState('E')) {
							cout << "プログラムを終了しますか？ <Y/N>" << endl;
							while (1) {
								if (GetAsyncKeyState('Y')) {
									cout << "プログラムを終了します。" << endl;
									Sleep(1000);
									return 0;
								}
								if (GetAsyncKeyState('N')) {
									cout << "キャンセルしました。" << endl;
									while (GetAsyncKeyState('N')) {
										Sleep(1);
									}
									break;
								}
							}
							while (GetAsyncKeyState('E')) {
								Sleep(1);
							}
						}

						// CTRL+ALT+S　→　ファイル保存
						if (GetAsyncKeyState('S')) {
							ostringstream oss;

							// 今日の日付を取得
							std::time_t rawtime;
							std::tm timeinfo;
							std::time(&rawtime);
							localtime_s(&timeinfo, &rawtime);
							int nowTime = (1900 + timeinfo.tm_year) * 10000 + (timeinfo.tm_mon + 1) * 100 + timeinfo.tm_mday;
							fileName = "data/" + to_string(nowTime) + ".csv";
							printf("ファイル：%s\n", fileName.c_str());

							if ((charCount - backSpaceCount) > 0) {
								wpm = (int)(charCount - backSpaceCount) / (mainTime / 60);
							}
							charCountR = 0;
							backSpaceCountR = 0;
							wpmR = 0;
							mainTimeR = 0;

							// 既に同じ名前のファイルがあるか確認
							try {
								ifstream ifs(fileName);
								string line;
								while (getline(ifs, line)) {

									vector<string> strvec = split(line, ',');

									charCountR = stoi(strvec.at(0));
									backSpaceCountR = stoi(strvec.at(1));
									wpmR = stoi(strvec.at(2));
									mainTimeR = stoi(strvec.at(3));


									for (int i = 4; i<30; i++) {
										int alphaI = i - 4;
										alphabetCountR[alphaI] = stoi(strvec.at(i));
									}
									for (int i = 30; i < 40; i++) {
										int numberI = i - 30;
										numberCountR[numberI] = stoi(strvec.at(i));
									}

								}
							}
							catch (std::exception& e) {
								std::cerr << e.what() << std::endl;
							}
							int countFlag = charCountR + backSpaceCountR + wpmR;

							// ファイルがあるときの処理
							if (countFlag != 0) {
								cout << endl << "既にファイルが存在します、上書きしますか？ Y/N" << endl;
								while (1) {
									if (GetAsyncKeyState('Y')) {
										cout << "イエスを選択しました。" << endl;

										ofstream ofs(fileName);
										ofs << charCount << "," << backSpaceCount << "," << wpm << "," << mainTime << ",";

										for (int i = 0; i < 26; i++) {
											ofs << alphabetCount[i] << ",";
										}
										for (int i = 0; i < 10; i++) {
											ofs << numberCount[i] << ",";
										}
										ofs << endl;

										printf("新規保存保存しました。%s\n", fileName.c_str());
										while (GetAsyncKeyState('Y')) {
											Sleep(1);
										}
										break;
									}
									if (GetAsyncKeyState('N')) {
										cout << "ノーを選択しました。" << endl;
										cout << "元々あったデータと数値を合算して保存しますか？ <Y/N>" << endl;
										Sleep(1000);
										if (GetAsyncKeyState('Y')) {
											cout << "イエスを選択しました。" << endl;

											sumChar = charCount + charCountR;
											sumSpace = backSpaceCount + backSpaceCountR;
											sumTime = mainTime + mainTimeR;
											if ((sumChar - sumSpace) > 0) {
												sumWpm = (int)(sumChar - sumSpace) / (sumTime / 60);
											}
											for (int i = 0; i < 26; i++) {
												sumAlphabet[i] = alphabetCount[i] + alphabetCountR[i];
											}
											for (int i = 0; i < 10; i++) {
												sumNumber[i] = numberCount[i] + numberCountR[i];
											}

											ofstream ofs(fileName);
											ofs << sumChar << "," << sumSpace << "," << sumWpm << "," << sumTime << ",";

											for (int i = 0; i < 26; i++) {
												ofs << sumAlphabet[i] << ",";
											}
											for (int i = 0; i < 10; i++) {
												ofs << sumNumber[i] << ",";
											}
											ofs << endl;

											printf("\n元あったデータと合わせて保存しました。%s\n", fileName.c_str());

											while (GetAsyncKeyState('Y')) {
												Sleep(1);
											}
											break;
										}

										if (GetAsyncKeyState('N')) {
											cout << "ノーを選択しました。" << endl;
											cout << "保存を中止しました。" << endl;
											while (GetAsyncKeyState('N')) {
												Sleep(1);
											}
											break;
										}
									}
								}
							}
							else {
								// ファイルがない時の処理
								ofstream ofs(fileName);
								ofs << charCount << "," << backSpaceCount << "," << wpm << "," << mainTime << ",";
								for (int i = 0; i < 26; i++) {
									ofs << alphabetCount[i] << ",";
								}
								for (int i = 0; i < 10; i++) {
									ofs << numberCount[i] << ",";
								}
								ofs << endl;

								printf("同じ名前のファイルがないので、新しく保存しました。%s\n", fileName.c_str());
							}

							while (GetAsyncKeyState('S')) {
								Sleep(1);
							}
						}

						// CTRL+ALT+R　→　ファイル読み込み
						if (GetAsyncKeyState('R')) {
							string inputName;
							cout << "ファイル名を入力してください(例：19990101)" << endl;
							cout << "入力したらエンターキーを押してください" << endl;
							cout << "ファイル名：";

							while (!GetAsyncKeyState(VK_RETURN)) {
								for (int i = 0; i < 10; i++) {
									char c = '0' + i;
									number[i] = GetAsyncKeyState(c);
									if (number[i] & 0x8000) {
										printf("%c", c);
										inputName.push_back(c);
										while (GetAsyncKeyState(c)) {
											Sleep(1);
										}
									}
								}
							}

							cout << endl;
							string inputFileName = "data/" + inputName + ".csv";

							charCountR = 0;
							backSpaceCountR = 0;
							wpmR = 0;
							mainTimeR = 0;

							try {
								ifstream ifs(inputFileName);
								string line;
								while (getline(ifs, line)) {

									vector<string> strvec = split(line, ',');

									charCountR = stoi(strvec.at(0));
									backSpaceCountR = stoi(strvec.at(1));
									wpmR = stoi(strvec.at(2));
									mainTimeR = stoi(strvec.at(3));

									for (int i = 4; i<30; i++) {
										int alphaI = i - 4;
										alphabetCountR[alphaI] = stoi(strvec.at(i));
									}
									for (int i = 30; i < 40; i++) {
										int numberI = i - 30;
										numberCountR[numberI] = stoi(strvec.at(i));
									}

								}
							}
							catch (std::exception& e) {
								std::cerr << e.what() << std::endl;
							}

							int countFlag = charCountR + backSpaceCountR + wpmR;
							// ファイルがあるときの処理
							if (countFlag != 0) {
								// 関数用の配列にコピー
								memcpy(resultAlphabet, alphabetCountR, sizeof(alphabetCountR));
								memcpy(resultNumber, numberCountR, sizeof(numberCountR));
								// 表示する関数呼び出し
								resultData(resultNumber, resultAlphabet, charCountR, backSpaceCountR, wpmR, inputName, mainTimeR);
								// 初期化
								memset(resultAlphabet, 0, sizeof(resultAlphabet));
								memset(resultNumber, 0, sizeof(resultNumber));
							}
							else {
								cout << endl << "ファイルが存在しません。" << endl;
							}
							while (GetAsyncKeyState('R')) {
								Sleep(1);
							}
						}

						// オートセーブ機能
						if (GetAsyncKeyState('O')) {
							autoSaveFlag = !autoSaveFlag;
							if (autoSaveFlag) {
								cout << "オートセーブ機能を'オン'にしました。" << endl;
								cout << "保存先のデータより打鍵数が少ない場合は自動で合算して保存します。" << endl;
							}
							else {
								cout << "オートセーブ機能を'オフ'にしました。" << endl;
							}
							while (GetAsyncKeyState('O')) {
								Sleep(1);
							}
						}

						// 初期化機能
						if (GetAsyncKeyState('C')) {
							cout << "今日のデータを初期化しますか？ <Y/N>" << endl;
							while (GetAsyncKeyState('C')) {
								Sleep(1);
							}
							while (1) {
								if (GetAsyncKeyState('Y')) {
									charCount = 0;
									backSpaceCount = 0;
									wpm = 0;
									mainTime = 0;
									memset(alphabetCount, 0, sizeof(alphabetCount));
									memset(numberCount, 0, sizeof(numberCount));
									cout << "今日のデータを初期化しました。" << endl;
									while (GetAsyncKeyState('Y')) {
										Sleep(1);
									}
									break;
								}
								if (GetAsyncKeyState('N')) {
									cout << "キャンセルしました。" << endl;
									while (GetAsyncKeyState('N')) {
										Sleep(1);
									}
									break;
								}
							}
						}

						// コマンド一覧を表示
						if (GetAsyncKeyState('D')) {
							cout << "CTRL+ALT+L：現在のデータ表示" << endl;
							cout << "CTRL+ALT+E：プログラム終了" << endl;
							cout << "CTRL+ALT+S：データをファイルに保存" << endl;
							cout << "CTRL+ALT+R：ファイルを読み込み" << endl;
							cout << "CTRL+ALT+O：オートセーブ機能切り替え" << endl;
							cout << "CTRL+ALT+C：現在のデータを初期化" << endl;
							cout << "CTRL+ALT+D：コマンド一覧を表示" << endl;
							while (GetAsyncKeyState('D')) {
								Sleep(1);
							}
						}

						Sleep(1);
					}
				}
				Sleep(1);
			}
		}
	}
	return 0;
}