// CarPuzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include"game.h"
#include"Player.h"
#include "Board.h"
#include "ProblemSummary.h"
#include <iostream>

int main()
{
	int heursitcType;
	//std::cin >> heursitcType;
	Player p(static_cast<Heuristic>(2));
	std::vector<std::string> tests;
	tests.push_back("AA...OP..Q.OPXXQ.OP..Q..B...CCB.RRR."); // finished
	tests.push_back("A..OOOA..B.PXX.BCPQQQ.CP..D.EEFFDGG.");	
	tests.push_back(".............XXO...AAO.P.B.O.P.BCC.P");	//finised
	tests.push_back("O..P..O..P..OXXP....AQQQ..A..B..RRRB");	//finised
	tests.push_back("AA.O.BP..OQBPXXOQGPRRRQGD...EED...FF");
	tests.push_back("AA.B..CC.BOP.XXQOPDDEQOPF.EQ..F..RRR");
	tests.push_back(".ABBCD.A.ECD.XXE.F..II.F...H.....H..");
	tests.push_back("...AAO..BBCOXXDECOFFDEGGHHIPPPKKIQQQ");
	tests.push_back(".ABBCC.A.DEEXX.DOFPQQQOFP.G.OHP.G..H");
	tests.push_back("AAB.CCDDB..OPXX..OPQQQ.OP..EFFGG.EHH");
	tests.push_back("OAAP..O..P..OXXP....BQQQ..B..E..RRRE");
	tests.push_back("ABB..OA.P..OXXP..O..PQQQ....C.RRR.C.");
	tests.push_back("AABBC...D.CO.EDXXOPE.FFOP..GHHPIIGKK");
	tests.push_back("AAB.....B.CCDEXXFGDEHHFG..I.JJKKI...");
	tests.push_back(".AABB.CCDDOPQRXXOPQREFOPQREFGG.HHII.");
	tests.push_back("AABBCOD.EECODFPXXO.FPQQQ..P...GG....");
	tests.push_back("AOOO..A.BBCCXXD...EEDP..QQQPFGRRRPFG");
	tests.push_back("AABO..CCBO..PXXO..PQQQ..PDD...RRR...");
	tests.push_back("..ABB...A.J..DXXJ..DEEF..OOOF.......");
	tests.push_back("A..OOOABBC..XXDC.P..D..P..EFFP..EQQQ");
	tests.push_back("AABO..P.BO..PXXO..PQQQ...........RRR");
	tests.push_back("..AOOOB.APCCBXXP...D.PEEFDGG.HFQQQ.H");
	tests.push_back("..OOOP..ABBP..AXXP..CDEE..CDFF..QQQ.");
	tests.push_back("..ABB..CA...DCXXE.DFF.E.OOO.G.HH..G.");
	tests.push_back("AAB.CCDDB..OPXX.EOPQQQEOPF.GHH.F.GII");
	tests.push_back(".A.OOOBA.CP.BXXCPDERRRPDE.F..G..FHHG");
	tests.push_back("ABBO..ACCO..XXDO.P..DEEP..F..P..FRRR");
	tests.push_back("OOOA....PABBXXP...CDPEEQCDRRRQFFGG.Q");
	tests.push_back("OOO.P...A.P.XXA.PBCDDEEBCFFG.HRRRG.H");
	tests.push_back("O.APPPO.AB..OXXB..CCDD.Q.....QEEFF.Q");
	tests.push_back("AA.OOO...BCCDXXB.PD.QEEPFFQ..P..QRRR");
	tests.push_back("AAOBCC..OB..XXO...DEEFFPD..K.PHH.K.P");
	tests.push_back(".AR.BB.AR...XXR...IDDEEPIFFGHPQQQGHP");
	tests.push_back("A..RRRA..B.PXX.BCPQQQDCP..EDFFIIEHH.");
	tests.push_back("..OAAP..OB.PXXOB.PKQQQ..KDDEF.GG.EF.");
	tests.push_back("OPPPAAOBCC.QOBXX.QRRRD.Q..EDFFGGE...");
	tests.push_back("AAB.CCDDB.OPQXX.OPQRRROPQ..EFFGG.EHH");
	tests.push_back("A..OOOABBC..XXDC.R..DEER..FGGR..FQQQ");
	tests.push_back("..AOOO..AB..XXCB.RDDCEERFGHH.RFGII..");
	tests.push_back("OAA.B.OCD.BPOCDXXPQQQE.P..FEGGHHFII.");




	//std::vector<game> games;
	for (int i = 0; i < tests.size(); i++) {
		ProblemSummary Summary;
		game g(tests[i]);
		p.play(g,Summary);
		std::cout << g.getSolution() << std::endl;
		//Summary.printSummary();
	}













}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
