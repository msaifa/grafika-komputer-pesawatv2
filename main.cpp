    #include <stdlib.h>
    #include <GL/glut.h>
    #include <math.h>
    #include <time.h>
    #include <stdio.h>
    #include <string.h>

    typedef struct
    {
        float x;
        float y;
    }point2D_t;

    typedef struct
    {
        float v[3];
    }vector2D_t;

    typedef struct
    {
        float m[3][3];
    } matrix2D_t;

    typedef struct
    {
        float r, g, b;
    } color_t;

    int lampMerahSamping ;


    point2D_t Vector2Point1(vector2D_t vec)
    {
        point2D_t pnt;
        pnt.x = vec.v[0];
        pnt.y = vec.v[1];
        return pnt;
    }

    vector2D_t Point2Vector1(point2D_t pnt)
    {
        vector2D_t vec;
        vec.v[0] = pnt.x;
        vec.v[1] = pnt.y;
        vec.v[2] = 1.;
        return vec;
    }


    void setColor(float r, float g, float b)
    {
        glColor3f(r, g, b);
    }

    void setColor(color_t col)
    {
        glColor3f(col.r, col.g, col.b);
    }

    //fungsi untuk membuat titik
    void drawDot(float x, float y)
    {

        glPointSize(3);
        glBegin(GL_POINTS);
        glVertex2f(x, y);
        glEnd();
    }

    //fungsi untuk membuat garis
    void drawLine(float x1, float y1, float x2, float y2)
    {

        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    //fungsi yang digunakan untuk menggambar garis
    void drawPolyline(point2D_t p[], int n)
    {
        int i;
        glBegin(GL_LINE_STRIP);
        for (i = 0; i < n; i++)
            glVertex2f(p[i].x, p[i].y);
        glEnd();
    }

    //fungsi untuk menggambar garis dengan bentuk kurva tertutup
    void drawPolygon(point2D_t p[], int n)
    {
        int i;
        glBegin(GL_LINE_LOOP);
        for (i = 0; i < n; i++)
            glVertex2f(p[i].x, p[i].y);
        glEnd();
    }

    //fungsi untuk mewarnai area poligon dengan satu warna
    void fillPolygon(point2D_t pnt[], int n, color_t color)
    {
        int i;
        setColor(color);
        glBegin(GL_POLYGON);
        for (i = 0; i < n; i++) {
            glVertex2f(pnt[i].x, pnt[i].y);
        }
        glEnd();
    }

    //fungsi yang menghasilkan efek gradasi jika digunakan untuk mewarnai pada poligon
    void gradatePolygon(point2D_t p[], int n, color_t col[])
    {
        int i;
        glBegin(GL_POLYGON);
        for (i = 0; i < n; i++)
        {
            setColor(col[i]);
            glVertex2f(p[i].x, p[i].y);
        }
        glEnd();
    }

    //fungsi yang menghasilkan efek gradasi jika digunakan untuk mewarnai pada lingkaran
    void centerPolygon(point2D_t p[], point2D_t pc, color_t col, color_t colp, int n)
    {
        int i;
        glBegin(GL_LINES);
        for (i = 0; i < n; i++)
        {
            setColor(colp);
            glVertex2f(pc.x, pc.y);
            setColor(col);
            glVertex2f(p[i].x, p[i].y);
        }
        glEnd();
    }


    // Definisi dan pengolahan matrik
    matrix2D_t createIdentity(void) {
        matrix2D_t u;
        int i, j;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) u.m[i][j] = 0.;
            u.m[i][i] = 1.;
        }
        return u;
    }

    // untuk membuat efek translasi(bergerak)
    matrix2D_t translationMTX(float dx, float dy)
    {
        matrix2D_t trans = createIdentity();
        trans.m[0][2] = dx;
        trans.m[1][2] = dy;
        return trans;
    }

    // untuk membuat efek scaling(berubah ukuran)
    matrix2D_t scalingMTX(float mx, float my)
    {
        matrix2D_t scale = createIdentity();
        scale.m[0][0] = mx;
        scale.m[1][1] = my;
        return scale;
    }

    // untuk membuat efek rotasi(berputar)
    matrix2D_t rotationMTX(float theta)
    {
        matrix2D_t rotate = createIdentity();
        float cs = cos(theta);
        float sn = sin(theta);
        rotate.m[0][0] = cs; rotate.m[0][1] = -sn;
        rotate.m[1][0] = sn; rotate.m[1][1] = cs;
        return rotate;
    }

    matrix2D_t operator * (matrix2D_t a, matrix2D_t b)
    {
        matrix2D_t c;//c=a*b
        int i, j, k;
        for (i = 0; i < 3; i++) for (j = 0; j < 3; j++) {
            c.m[i][j] = 0;
            for (k = 0; k < 3; k++)
                c.m[i][j] += a.m[i][k] * b.m[k][j];
        }
        return c;
    }

    vector2D_t operator * (matrix2D_t a, vector2D_t b)
    {
        vector2D_t c;//c=a*b
        int i, j;
        for (i = 0; i < 3; i++) {
            c.v[i] = 0;
            for (j = 0; j < 3; j++)
                c.v[i] += a.m[i][j] * b.v[j];
        }
        return c;
    }

    // fungsi untuk menampilkan tulisan
    void bitmap_output(int x, int y, char *string, void *font, color_t color)
    {
        int len, i;
        setColor(color);
        glRasterPos2f(x, y);
        len = (int)strlen(string);
        for (i = 0; i < len; i++) {
            glutBitmapCharacter(font, string[i]);
        }
    }

    // fungsi yang digunakan untuk membuat ellipse
    static void createEllipse(point2D_t p[], point2D_t p0, float r1, float r2)
    {
        for (int i = 0; i < 360; i++) {
            p[i].x = p0.x + r1 * cos(i / 57.3);
            p[i].y = p0.y + r2 * sin(i / 57.3);
        }
    }


    void background(){
        point2D_t savana[4] = { {0,0},{1200,0},{1200,166},{0,166} };
        point2D_t
            gabahA[360], kgabahA = { 321,166 },
            gabahB[360], kgabahB = { 383,164 },
            gabahC[360], kgabahC = { 829,164 },
            gabahD[360], kgabahD = { 872,164 },
            gabahE[360], kgabahE = { 941,164 },
            matahari[360], kmatahari = {1168, 670} ;

        color_t hijaurumput = { 0.48,0.67,0.13 };
        color_t wgabahA = { 0.19,0.61,0.19 } ;
        color_t wgabahB = { 0.83,0.86,0.18 } ;
        color_t wgabahC = { 0.81,0.8,0.16 } ;
        color_t wmatahari = { 1,0.8,0.3 } ;

        createEllipse(gabahE, kgabahE, 90, 90);
        fillPolygon(gabahE, 360, wgabahB);
        createEllipse(gabahC, kgabahC, 110, 110);
        fillPolygon(gabahC, 360, wgabahC);
        createEllipse(gabahB, kgabahB, 110, 110);
        fillPolygon(gabahB, 360, wgabahB);
        createEllipse(gabahA, kgabahA, 60, 50);
        fillPolygon(gabahA, 360, wgabahA);
        createEllipse(gabahD, kgabahD, 70, 80);
        fillPolygon(gabahD, 360, wgabahA);
        createEllipse(matahari, kmatahari, 80, 80);
        fillPolygon(matahari, 360, wmatahari);

        fillPolygon(savana, 4, hijaurumput);
    }


    void pepohonan(){
        point2D_t pohon1[8] = { {80,150},{81,258},{76,267},{58,271},{90,269},{103,277},{94,261},{100,150} };
        point2D_t pohon1DaunA[43] = { {80,232},{71,231},{60,237},{52,235},{46,239},{43,248},{34,249},{25,256},{25,265},{19,268},{13,280},{15,294},{19,296},{14,311},{15,320},{21,327},{20,340},{24,350},{31,348},{31,361},{39,370},{49,371},{57,381},{70,383},{77,381} ,{85,388} ,{96,385} ,{102,377} ,{116,376} ,{122,364} ,{141,350} ,{140,340} ,{147,325} ,{147,309} ,{149,286} ,{143,276} ,{135,277} ,{123,262} ,{126,256} ,{120,243} ,{120,243} ,{105,236} ,{96,236} } ;

        point2D_t pohon2A[6] = { {219,151} ,{232,150} ,{231,274} ,{236,288} ,{225,292} ,{222,275}} ;
        point2D_t pohon2B[3] = { {221,274},{212,291},{224,282} } ;
        point2D_t pohon2C[3] = { {225,285},{227,345},{230,285} } ;
        point2D_t pohon2D[3] = { {230,290},{245,310},{230,272} } ;
        point2D_t
            pohon2DaunA[360], kpohon2DaunA = { 200,291 },
            pohon2DaunB[360], kpohon2DaunB = { 220,340 },
            pohon2DaunC[360], kpohon2DaunC = { 254,304 } ;

        point2D_t pohon3A[7] = {{1002,150} ,{1021,149} ,{1019,251} ,{1014,262} ,{1011,297} ,{1007,265} ,{1003,256}} ;
        point2D_t pohon3B[3] = {{1004,255}, {988,275}, {1005,263}} ;
        point2D_t pohon3C[3] = {{1016,255}, {1036,266}, {1019,247}} ;
        point2D_t
            pohon3DaunA[360], kpohon3DaunA = { 1010,313 } ;

        point2D_t pohon4A[5] = {{1131,150} ,{1145,150} ,{1144,291} ,{1139,336} ,{1133,270}} ;
        point2D_t pohon4B[3] = {{1141,290},{1158,314},{1143,274}} ;
        point2D_t pohon4C[3] = {{1132,273} ,{1121,295} ,{1135,282}} ;
        point2D_t
            pohon4DaunA[360], kpohon4DaunA = { 1112,290 },
            pohon4DaunB[360], kpohon4DaunB = { 1131,341 },
            pohon4DaunC[360], kpohon4DaunC = { 1170,305 } ;

        // warna warna
        color_t wpohon1 = { 0.46,0.29,0.08 };
        color_t wpohon1DaunA = { 0.69,0.74,0.18 };
        color_t wpohon2Daun = { 0.52,0.76,0.51 } ;
        color_t wpohon3Daun = { 0.29,0.5,0.24 } ;

        fillPolygon(pohon1DaunA, 43, wpohon1DaunA);
        fillPolygon(pohon1, 8, wpohon1);

        // pohon 2
        createEllipse(pohon2DaunB, kpohon2DaunB, 50, 50);
        createEllipse(pohon2DaunA, kpohon2DaunA, 50, 50);
        createEllipse(pohon2DaunC, kpohon2DaunC, 50, 60);
        fillPolygon(pohon2DaunB, 360, wpohon2Daun);
        fillPolygon(pohon2DaunA, 360, wpohon2Daun);
        fillPolygon(pohon2DaunC, 360, wpohon2Daun);
        fillPolygon(pohon2A, 6, wpohon1);
        fillPolygon(pohon2B, 3, wpohon1);
        fillPolygon(pohon2C, 3, wpohon1);
        fillPolygon(pohon2D, 3, wpohon1);

        //pohon 3
        createEllipse(pohon3DaunA, kpohon3DaunA, 50, 90);
        fillPolygon(pohon3DaunA, 360, wpohon3Daun);
        fillPolygon(pohon3A, 7, wpohon1);
        fillPolygon(pohon3B, 3, wpohon1);
        fillPolygon(pohon3C, 3, wpohon1);

        //pohon 4
        createEllipse(pohon4DaunB, kpohon4DaunB, 50, 50);
        createEllipse(pohon4DaunA, kpohon4DaunA, 50, 50);
        createEllipse(pohon4DaunC, kpohon4DaunC, 50, 60);
        fillPolygon(pohon4DaunB, 360, wpohon2Daun);
        fillPolygon(pohon4DaunA, 360, wpohon2Daun);
        fillPolygon(pohon4DaunC, 360, wpohon2Daun);
        fillPolygon(pohon4A, 5, wpohon1);
        fillPolygon(pohon4B, 3, wpohon1);
        fillPolygon(pohon4C, 3, wpohon1);
    }

    void rumah(){
        point2D_t tembok[5] = {{463,94} ,{786,94} ,{790,329} ,{627,451} ,{456,325}} ;
        point2D_t bayangantembok[4] = {{458,287} ,{644,421} ,{622,448} ,{460,329}} ;
        point2D_t atap1[4] = {{417,327},{439,296},{650,450},{616,475}} ;
        point2D_t atap2[4] = {{808,295},{831,327},{623,479},{592,453}} ;
        point2D_t cerobong[4] = {{704,450}, {704,380}, {756,376}, {756,450}} ;
        point2D_t tutupcerobong[4] = {{693,471} ,{693,443} ,{767,443} ,{767,471}} ;
        point2D_t bayanganbawah[4] = {{463,94} ,{786,94},{786,140},{463,140}};
        point2D_t pintuDasar[4] = {{500,91}, {609,91}, {609,278}, {500,278}} ;
        point2D_t pintuDasar1[4] = {{506,98},{506,273},{601,273},{601,100}} ;
        point2D_t pintuDasar2[4] = {{515,258} ,{592,258} ,{592,120} ,{515,120}} ;
        point2D_t pintuHiasan1[4] = {{550,258},{558,258},{558,120},{550,120}} ;
        point2D_t pintuhiasan2[4] = {{515,220}, {592,219}, {592,208}, {515,208}} ;
        point2D_t pintuhiasan3[4] = {{515,170}, {592,170}, {592,160}, {515,160}} ;
        point2D_t
            gagang[360], kgagang = { 580,182 } ;
        point2D_t jendeladasar[4] = {{644,278}, {752,278}, {751,166}, {644,166}} ;
        point2D_t jendela[4] = {{652,267},{744,268},{744,178},{652,178}} ;
        point2D_t hiasanjendela[4] = {{693,273},{693,177},{702,177},{702,273}} ;
        point2D_t gawangjendela[4] = {{634,170}, {634,163}, {762,163}, {762,170}} ;

        point2D_t pagar1Hor1[4] = {{360,175}, {360,160}, {471,160}, {471,175}} ;
        point2D_t pagar1Hor2[4] = {{361,134},{361,118},{472,118},{472,134}} ;
        point2D_t pager1Ver1[5] = {{380,200},{370,190},{368,97},{390,97},{390,190}} ;
        point2D_t pager1Ver2[5] = {{415,201},{405,188},{405,97},{426,97},{426,188}} ;
        point2D_t pager1Ver3[5] = {{452,200},{442,190},{441,97},{462,97},{462,190}} ;

        point2D_t pager2Hor1[4] = {{773,175},{772,158},{884,158},{884,175}} ;
        point2D_t pager2Hor2[4] = {{773,119},{884,119},{884,133},{773,133}} ;
        point2D_t pager2Ver1[5] = {{794,200},{783,188},{783,97},{805,97},{805,188}} ;
        point2D_t pager2Ver2[5] = {{830,201},{821,189},{821,97},{842,97},{841,188}} ;
        point2D_t pager2Ver3[5] = {{866,199},{855,188},{856,97},{875,97},{875,189}} ;


        //warna warna
        color_t wtembok = {1,0.74,0.27} ;
        color_t wbayangTembok = {0.88, 0.62, 0.25} ;
        color_t watap = {0.22,0.13,0.05} ;
        color_t wcerobong = {0.36,0.21,0.07} ;
        color_t wdasrpintu = {0.37,0.11,0.04} ;
        color_t whiasanpintu = {0.6, 0.25,0.01} ;
        color_t putih = {0.9,0.9,0.9} ;
        color_t putihtulang = {0.9,0.89,0.95} ;

        fillPolygon(tembok, 5, wtembok);
        fillPolygon(bayangantembok, 4, wbayangTembok);
        fillPolygon(cerobong, 4, wcerobong);
        fillPolygon(atap1, 4, watap);
        fillPolygon(atap2, 4, watap);
        fillPolygon(tutupcerobong, 4, watap);
        fillPolygon(bayanganbawah, 4, wbayangTembok);

        // pintu
        fillPolygon(pintuDasar, 4, watap);
        fillPolygon(pintuDasar1, 4, wdasrpintu);
        fillPolygon(pintuDasar2, 4, whiasanpintu);
        fillPolygon(pintuHiasan1, 4, wdasrpintu);
        fillPolygon(pintuhiasan2, 4, wdasrpintu);
        fillPolygon(pintuhiasan3, 4, wdasrpintu);
        createEllipse(gagang, kgagang, 7, 7);
        fillPolygon(gagang, 360, putih);

        //jendelea
        fillPolygon(jendeladasar, 4, watap);
        fillPolygon(jendela, 4, putih);
        fillPolygon(hiasanjendela, 4, watap);
        fillPolygon(gawangjendela, 4, wdasrpintu);

        //pagar
        fillPolygon(pagar1Hor1, 4, putihtulang);
        fillPolygon(pagar1Hor2, 4, putihtulang);
        fillPolygon(pager1Ver1, 5, putih);
        fillPolygon(pager1Ver2, 5, putih);
        fillPolygon(pager1Ver3, 5, putih);

        fillPolygon(pager2Hor1, 4, putihtulang);
        fillPolygon(pager2Hor2, 4, putihtulang);
        fillPolygon(pager2Ver1, 5, putih);
        fillPolygon(pager2Ver2, 5, putih);
        fillPolygon(pager2Ver3, 5, putih);

    }

    void jalan(){
        point2D_t jalanDasar[4] = {{0,68},{1200,68},{1200,0},{0,0}} ;
        point2D_t jalanAspal[4] = {{0,55},{1200,55},{1200,0},{0,0}} ;
        point2D_t marka1[4] = {{29,28},{29,20},{60,20},{60,28}} ;
        point2D_t marka2[4] = {{97,28},{97,20},{128,20},{128,28}} ;
        point2D_t marka3[4] = {{165,28},{165,20},{196,20},{196,28}} ;
        point2D_t marka4[4] = {{233,28},{233,20},{264,20},{264,28}} ;
        point2D_t marka5[4] = {{301,28},{301,20},{332,20},{332,28}} ;
        point2D_t marka6[4] = {{369,28},{369,20},{400,20},{400,28}} ;
        point2D_t marka7[4] = {{437,28},{437,20},{468,20},{468,28}} ;
        point2D_t marka8[4] = {{505,28},{505,20},{536,20},{536,28}} ;
        point2D_t marka9[4] = {{573,28},{573,20},{604,20},{604,28}} ;
        point2D_t marka10[4] = {{641,28},{641,20},{672,20},{672,28}} ;
        point2D_t marka11[4] = {{709,28},{709,20},{740,20},{740,28}} ;
        point2D_t marka12[4] = {{777,28},{777,20},{808,20},{808,28}} ;
        point2D_t marka13[4] = {{845,28},{845,20},{876,20},{876,28}} ;
        point2D_t marka14[4] = {{913,28},{913,20},{944,20},{944,28}} ;
        point2D_t marka15[4] = {{981,28},{981,20},{1012,20},{1012,28}} ;
        point2D_t marka16[4] = {{1049,28},{1049,20},{1080,20},{1080,28}} ;
        point2D_t marka17[4] = {{1117,28},{1117,20},{1148,20},{1148,28}} ;
        //point2D_t marka18[4] = {{1185,28},{1185,20},{1216,20},{1216,28}} ;
        point2D_t jalanDasar2[4] = {{1137,54},{1137,162},{1151,162},{1151,54}} ;
        point2D_t jalanAspal2[4] = {{1149,160},{1149,0},{1200,0},{1200,160}} ;
        point2D_t marka18[4] = {{1178,133},{1178,99},{1185,99},{1185,133}} ;

        color_t putih = {0.9,0.9,0.9} ;
        color_t aspal = {0.4,0.4,0.38} ;

        // jalan 1
        fillPolygon(jalanDasar,4,putih);
        fillPolygon(jalanAspal,4,aspal);
        fillPolygon(marka1,4,putih);
        fillPolygon(marka2,4,putih);
        fillPolygon(marka3,4,putih);
        fillPolygon(marka4,4,putih);
        fillPolygon(marka5,4,putih);
        fillPolygon(marka6,4,putih);
        fillPolygon(marka7,4,putih);
        fillPolygon(marka8,4,putih);
        fillPolygon(marka9,4,putih);
        fillPolygon(marka10,4,putih);
        fillPolygon(marka11,4,putih);
        fillPolygon(marka12,4,putih);
        fillPolygon(marka13,4,putih);
        fillPolygon(marka14,4,putih);
        fillPolygon(marka15,4,putih);
        fillPolygon(marka16,4,putih);
        fillPolygon(marka17,4,putih);

        //jalan 2 dulu
        fillPolygon(jalanDasar2,4,putih);
        fillPolygon(jalanAspal2,4,aspal);
        fillPolygon(marka18,4,putih);
    }

    void lantas(){
        matrix2D_t mat;
        vector2D_t vec;
        static float tick = 0;
        int i;

        point2D_t kotak[4] = {{1101,157},{1102,107},{1124,107},{1124,157}};
        point2D_t tiang[4] = {{1108,110},{1108,68},{1119,68},{1119,110}} ;
        point2D_t
            lampu1[360], klampu1 = { 1112,147 },
            lampu2[360], klampu2 = { 1112,131 },
            lampu3[360], klampu3 = { 1112,116 },
            lampu4[360], klampu4 = { 1105,147 },
            lampu5[360], klampu5 = { 1105,131 },
            lampu6[360], klampu6 = { 1105,116 } ;

        color_t wKotak = {0.07,0.12,0.12} ;
        color_t wTiang = {0.51,0.51,0.51} ;
        color_t merah = {1,0.2,0.4} ;
        color_t kuning = {1,0.92,0.24} ;
        color_t hijau = {0,1,0.43} ;
        color_t merahb = {0.6,0,0} ;
        color_t kuningb = {0.6,0.52,0} ;
        color_t hijaub = {0,0.6,0} ;

        createEllipse(lampu5, klampu5, 7, 7);
        fillPolygon(lampu5, 360, kuningb);

        if (fmod(tick,20) < 10){
            lampMerahSamping = 1 ;
            createEllipse(lampu6, klampu6, 7, 7);
            fillPolygon(lampu6, 360, hijaub);

            createEllipse(lampu4, klampu4, 7, 7);
            fillPolygon(lampu4, 360, merah);
        } else {
            lampMerahSamping = 0 ;
            createEllipse(lampu6, klampu6, 7, 7);
            fillPolygon(lampu6, 360, hijau);

            createEllipse(lampu4, klampu4, 7, 7);
            fillPolygon(lampu4, 360, merahb);
        }

        fillPolygon(tiang,4,wTiang);
        fillPolygon(kotak,4,wKotak);

        createEllipse(lampu2, klampu2, 7, 7);
        fillPolygon(lampu2, 360, kuningb);

        if (fmod(tick,20) < 10){
        //depan
            createEllipse(lampu1, klampu1, 7, 7);
            fillPolygon(lampu1, 360, merahb);

            createEllipse(lampu3, klampu3, 7, 7);
            fillPolygon(lampu3, 360, hijau);
        } else {
        // depan
            createEllipse(lampu1, klampu1, 7, 7);
            fillPolygon(lampu1, 360, merah);

            createEllipse(lampu3, klampu3, 7, 7);
            fillPolygon(lampu3, 360, hijaub);
        }

        tick += 0.03 ;

    }

    void mobil(){
        matrix2D_t mat;
        vector2D_t vec;
        static float tSpeed = 0;
        int i;

        point2D_t mainBody[11] = {{23,56},{17,78},{20,107},{26,124},{37,133},{78,136},{112,135},{137,131},{173,98},{205,81},{211,54}} ;
        point2D_t bemper[9] = {{19,65},{14,62},{13,54},{18,44},{18,44},{212,44},{217,49},{218,56},{211,61}} ;
        point2D_t gagang1[4] = {{74,92},{74,88},{86,88},{86,92}} ;
        point2D_t gagang2[4] = {{123,92},{123,88},{135,88},{135,92}} ;
        point2D_t jendela[9] = {{28,101},{160,98},{133,125},{116,125},{116,128},{67,128},{42,126},{35,122},{31,115}} ;
        point2D_t batasJen1[4] = {{67,131},{67,98},{71,98},{71,131}} ;
        point2D_t batasJen2[4] = {{111,131},{111,98},{115,98},{115,131}} ;

        color_t mainColor = {0.92,0,0.09} ;
        color_t wbemper = {0.61,0.61,0.56} ;
        color_t wjendela = {0.17,0.17,0.17} ;
        color_t hitam = {0,0,0} ;

        mat = translationMTX(0 + tSpeed, 0);
        for (i = 0; i < 11; i++)
        {
            vec = Point2Vector1(mainBody[i]);
            vec = mat * vec;
            mainBody[i] = Vector2Point1(vec);
        }
        for (i = 0; i < 9; i++)
        {
            vec = Point2Vector1(bemper[i]);
            vec = mat * vec;
            bemper[i] = Vector2Point1(vec);

            vec = Point2Vector1(jendela[i]);
            vec = mat * vec;
            jendela[i] = Vector2Point1(vec);
        }
        for (i = 0; i < 4; i++)
        {
            vec = Point2Vector1(gagang1[i]);
            vec = mat * vec;
            gagang1[i] = Vector2Point1(vec);

            vec = Point2Vector1(gagang2[i]);
            vec = mat * vec;
            gagang2[i] = Vector2Point1(vec);

            vec = Point2Vector1(batasJen1[i]);
            vec = mat * vec;
            batasJen1[i] = Vector2Point1(vec);

            vec = Point2Vector1(batasJen2[i]);
            vec = mat * vec;
            batasJen2[i] = Vector2Point1(vec);
        }

        fillPolygon(mainBody,11,mainColor);
        fillPolygon(bemper,8,wbemper);
        fillPolygon(gagang1,4,wbemper);
        fillPolygon(gagang2,4,wbemper);
        fillPolygon(jendela,9,wjendela);
        fillPolygon(batasJen1,4,mainColor);
        fillPolygon(batasJen2,4,mainColor);

        point2D_t
            roda1luar[360], kroda1luar = { 59,51 },
            roda2luar[360], kroda2luar = { 168,51 },
            roda1dalam[360], kroda1dalam = { 59,51 },
            roda2dalam[360], kroda2dalam = { 168,51 } ;

        createEllipse(roda1luar, kroda1luar, 23, 23);
        createEllipse(roda1dalam, kroda1dalam, 13, 13);
        createEllipse(roda2luar, kroda2luar, 23, 23);
        createEllipse(roda2dalam, kroda2dalam, 13, 13);
        for (i = 0; i < 360; i++)
        {
            vec = Point2Vector1(roda1luar[i]);
            vec = mat * vec;
            roda1luar[i] = Vector2Point1(vec);

            vec = Point2Vector1(roda1dalam[i]);
            vec = mat * vec;
            roda1dalam[i] = Vector2Point1(vec);

            vec = Point2Vector1(roda2luar[i]);
            vec = mat * vec;
            roda2luar[i] = Vector2Point1(vec);

            vec = Point2Vector1(roda2dalam[i]);
            vec = mat * vec;
            roda2dalam[i] = Vector2Point1(vec);
        }

        fillPolygon(roda1luar, 360, hitam);
        fillPolygon(roda1dalam, 360, wbemper);
        fillPolygon(roda2luar, 360, hitam);
        fillPolygon(roda2dalam, 360, wbemper);

        if (!(lampMerahSamping==1 && tSpeed == 900)){
            tSpeed +=3 ;
        }


        if (tSpeed >= 1300){
            tSpeed = -300 ;
        }

    }

    void asapRumah(){
        matrix2D_t mat;
        vector2D_t vec;
        static float tSpeed1,tSpeed2,tSpeed3,tSpeed4,tSpeed5,tSpeed6 = 0;
        int i;

        point2D_t
            asap1[360], kasap1 = { 754,515 },
            asap2[360], kasap2 = { 706,526 },
            asap3[360], kasap3 = { 749,570 },
            asap4[360], kasap4 = { 692,589 },
            asap5[360], kasap5 = { 750,652 },
            asap6[360], kasap6 = { 709,626 } ;

        color_t wasap = {0.53,0.5,0.5} ;

        createEllipse(asap1, kasap1, 40, 40);
        createEllipse(asap2, kasap2, 40, 40);
        createEllipse(asap3, kasap3, 40, 40);
        createEllipse(asap4, kasap4, 40, 40);
        createEllipse(asap5, kasap5, 40, 40);
        createEllipse(asap6, kasap6, 40, 40);

        mat = translationMTX(0, 0 + tSpeed1);
        for (i = 0; i < 360; i++){
            vec = Point2Vector1(asap1[i]);
            vec = mat * vec;
            asap1[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0, 0 + tSpeed2);
        for (i = 0; i < 360; i++){
            vec = Point2Vector1(asap2[i]);
            vec = mat * vec;
            asap2[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0, 0 + tSpeed3);
        for (i = 0; i < 360; i++){
            vec = Point2Vector1(asap3[i]);
            vec = mat * vec;
            asap3[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0, 0 + tSpeed4);
        for (i = 0; i < 360; i++){
            vec = Point2Vector1(asap4[i]);
            vec = mat * vec;
            asap4[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0, 0 + tSpeed5);
        for (i = 0; i < 360; i++){
            vec = Point2Vector1(asap5[i]);
            vec = mat * vec;
            asap5[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0, 0 + tSpeed6);
        for (i = 0; i < 360; i++){
            vec = Point2Vector1(asap6[i]);
            vec = mat * vec;
            asap6[i] = Vector2Point1(vec);
        }

        fillPolygon(asap1, 360, wasap);
        fillPolygon(asap2, 360, wasap);
        fillPolygon(asap3, 360, wasap);
        fillPolygon(asap4, 360, wasap);
        fillPolygon(asap5, 360, wasap);
        fillPolygon(asap6, 360, wasap);

        tSpeed1+=2 ;
        tSpeed2+=2 ;
        tSpeed3+=2 ;
        tSpeed4+=2 ;
        tSpeed5+=2 ;
        tSpeed6+=2 ;

        if (tSpeed1 >= 300){
            tSpeed1 = -10 ;
        }
        if (tSpeed2 >= 300){
            tSpeed2 = -25 ;
        }
        if (tSpeed3 >= 300){
            tSpeed3 = -55 ;
        }
        if (tSpeed4 >= 300){
            tSpeed4 = -75 ;
        }
        if (tSpeed5 >= 300){
            tSpeed5 = -145 ;
        }
        if (tSpeed6 >= 300){
            tSpeed6 = -125 ;
        }

    }

    void awan(){
        matrix2D_t mat;
        vector2D_t vec;
        static float tSpeed1,tSpeed2,tSpeed3,tSpeed4 = 0;
        int i;

        point2D_t awan1[23] = {{42,569},{45,584},{60,600},{77,598},{91,612},{103,618},{115,614},{127,631},{133,637},{165,634},{182,617},{185,598},{195,593},{212,606},{227,609},{243,598},{257,609},{273,612},{292,608},{303,597},{306,585},{322,584},{334,572}} ;
        point2D_t awan2[23] = {{142,619},{145,634},{160,650},{177,648},{191,662},{203,668},{215,664},{227,681},{233,687},{265,684},{282,667},{285,648},{295,643},{312,656},{327,659},{343,648},{357,659},{373,662},{392,658},{403,647},{406,635},{422,634},{434,622}} ;
        point2D_t awan3[23] = {{92,469},{95,484},{110,500},{127,498},{141,512},{153,518},{165,514},{177,531},{183,537},{215,534},{232,517},{235,498},{245,493},{262,506},{277,509},{293,498},{307,509},{323,512},{342,508},{353,497},{356,485},{372,484},{384,472}} ;
        point2D_t awan4[23] = {{542,644},{545,659},{560,675},{577,673},{591,687},{603,693},{615,689},{627,706},{633,712},{665,709},{682,692},{685,673},{695,668},{712,681},{727,684},{743,673},{757,684},{773,687},{792,683},{803,672},{806,660},{822,659},{834,647}} ;

        color_t wAwan1 = { 0.9, 0.93, 1} ;
        color_t wAwan2 = { 1, 1, 1} ;
        color_t wAwan3 = {0.95,0.95,1} ;

        mat = translationMTX(0 + tSpeed1, 0);
        for (i = 0; i < 23; i++){
            vec = Point2Vector1(awan1[i]);
            vec = mat * vec;
            awan1[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0 + (tSpeed2*-1), 0);
        for (i = 0; i < 23; i++){
            vec = Point2Vector1(awan2[i]);
            vec = mat * vec;
            awan2[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0 + tSpeed3, 0);
        for (i = 0; i < 23; i++){
            vec = Point2Vector1(awan3[i]);
            vec = mat * vec;
            awan3[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0 + tSpeed4, 0);
        for (i = 0; i < 23; i++){
            vec = Point2Vector1(awan4[i]);
            vec = mat * vec;
            awan4[i] = Vector2Point1(vec);
        }

        fillPolygon(awan1,23,wAwan1) ;
        fillPolygon(awan2,23,wAwan2) ;
        fillPolygon(awan3,23,wAwan3) ;
        fillPolygon(awan4,23,wAwan2) ;

        tSpeed1 += 2;
        tSpeed2 += 1;
        tSpeed3 += 4;
        tSpeed4 += 0.5 ;

        if (tSpeed1 >= 1200){
            tSpeed1 = -250 ;
        }

        if (tSpeed2 >= 500){
            tSpeed2 = -1200;
        }

        if (tSpeed3 >= 1200){
            tSpeed3 = -250 ;
        }

        if (tSpeed4 >= 700){
            tSpeed4 = -800 ;
        }
    }

    void pesawat(){
        matrix2D_t mat;
        vector2D_t vec;
        static float tSpeed,tSpeed2 = 0;
        int i;

        point2D_t dasar[9] = {{1189,444},{1184,437},{1179,435},{1125,432},{1111,435},{1114,439},{1122,443},{1130,450},{1146,451}} ;
        point2D_t sayapBelakang[5] = {{1167,445},{1178,462},{1183,462},{1186,461},{1184,442}};
        point2D_t sayap1[6] = {{1140,436},{1157,422},{1164,423},{1167,424},{1161,437},{1152,438}};
        point2D_t sayap2[5] = {{1175,452},{1183,448},{1187,448},{1190,450},{1185,453}} ;
        point2D_t jendela[6] = {{1123,443},{1131,445},{1146,444},{1146,447},{1132,447},{1127,447}} ;

        color_t wDasar = {1,0.57,0} ;
        color_t wDasar2 = {0.95,0.34,0} ;
        color_t wJendela = {0.31,0.31,0.38} ;

        mat = translationMTX(0 - tSpeed, 0+tSpeed2);
        for (i = 0; i < 9; i++)
        {
            vec = Point2Vector1(dasar[i]);
            vec = mat * vec;
            dasar[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0 - tSpeed, 0+tSpeed2);
        for (i = 0; i < 5; i++)
        {
            vec = Point2Vector1(sayapBelakang[i]);
            vec = mat * vec;
            sayapBelakang[i] = Vector2Point1(vec);

            vec = Point2Vector1(sayap2[i]);
            vec = mat * vec;
            sayap2[i] = Vector2Point1(vec);
        }

        mat = translationMTX(0 - tSpeed, 0+tSpeed2);
        for (i = 0; i < 6; i++)
        {
            vec = Point2Vector1(sayap1[i]);
            vec = mat * vec;
            sayap1[i] = Vector2Point1(vec);

            vec = Point2Vector1(jendela[i]);
            vec = mat * vec;
            jendela[i] = Vector2Point1(vec);
        }

        fillPolygon(dasar,9,wDasar);
        fillPolygon(sayapBelakang,5,wDasar);
        fillPolygon(sayap2,5,wDasar2);
        fillPolygon(sayap1,6,wDasar2);
        fillPolygon(jendela,6,wJendela);

        tSpeed += 0.5 ;
        tSpeed2 += 0.15 ;
    }

    void bintang(){
        matrix2D_t mat;
        vector2D_t vec;
        static float tSpeed,tSpeed2 = 0;
        int i;

        point2D_t bin1A[3] = {{14,25},{9,12},{19,12}} ;
        point2D_t bin1B[3] = {{4,16},{13,8},{13,18}} ;
        point2D_t bin1C[3] = {{8,5},{19,11},{10,17}} ;
        point2D_t bin1D[3] = {{20,5},{9,11},{18,17}} ;
        point2D_t bin1E[3] = {{24,16},{14,8},{11,18}} ;

        color_t kuning = {1,1,0} ;

        mat = translationMTX(0 + (tSpeed*2), 0 + tSpeed);
        for (i = 0; i < 3; i++)
        {
            vec = Point2Vector1(bin1A[i]);
            vec = mat * vec;
            bin1A[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1B[i]);
            vec = mat * vec;
            bin1B[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1C[i]);
            vec = mat * vec;
            bin1C[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1D[i]);
            vec = mat * vec;
            bin1D[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1E[i]);
            vec = mat * vec;
            bin1E[i] = Vector2Point1(vec);
        }

        fillPolygon(bin1A,3,kuning);
        fillPolygon(bin1B,3,kuning);
        fillPolygon(bin1C,3,kuning);
        fillPolygon(bin1D,3,kuning);
        fillPolygon(bin1E,3,kuning);

        tSpeed += 1;
    }

     void bintang2(){
        matrix2D_t mat;
        vector2D_t vec;
        static float tSpeed,tSpeed2 = 0;
        int i;

        point2D_t bin1A[3] = {{14,685},{9,672},{19,672}} ;
        point2D_t bin1B[3] = {{4,676},{13,668},{13,678}} ;
        point2D_t bin1C[3] = {{8,665},{19,671},{10,677}} ;
        point2D_t bin1D[3] = {{20,665},{9,671},{18,677}} ;
        point2D_t bin1E[3] = {{24,676},{14,668},{11,678}} ;

        color_t kuning = {1,1,0} ;

        mat = translationMTX(0 + (tSpeed*2), 0 - tSpeed);
        for (i = 0; i < 3; i++)
        {
            vec = Point2Vector1(bin1A[i]);
            vec = mat * vec;
            bin1A[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1B[i]);
            vec = mat * vec;
            bin1B[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1C[i]);
            vec = mat * vec;
            bin1C[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1D[i]);
            vec = mat * vec;
            bin1D[i] = Vector2Point1(vec);

            vec = Point2Vector1(bin1E[i]);
            vec = mat * vec;
            bin1E[i] = Vector2Point1(vec);
        }

        fillPolygon(bin1A,3,kuning);
        fillPolygon(bin1B,3,kuning);
        fillPolygon(bin1C,3,kuning);
        fillPolygon(bin1D,3,kuning);
        fillPolygon(bin1E,3,kuning);

        tSpeed += 2;
    }

    void display(void)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // tempat meletakkan objek

        background();

        pesawat();

        awan();

        pepohonan();

        rumah();

        jalan() ;

        lantas();

        mobil();

        asapRumah() ;

        bintang();
        bintang2();

        glutSwapBuffers();
    }

    int main(int argc, char ** argv)
    {
        // insert code here...
        glutInit(&argc, argv); //inisialisasi toolkit
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        glutInitWindowPosition(100, 100);
        glutInitWindowSize(1200, 700); // besar ukuran windows
        glutCreateWindow("SAIF ALIKHAN");
        glClearColor(0.72,1,1,0); // warna background biru muda/tua
        gluOrtho2D(0.,1200.,0.,700.);
        glutIdleFunc(display);
        glutDisplayFunc(display);
        glutMainLoop();
        return 0;
    }
