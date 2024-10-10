#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

int FlatShaded = 0, Wireframed = 0; // 모드 토글
int ViewX = 0, ViewY = 0; // 마우스 좌표
int ViewMode = -1;
int vX = 0, vY = 0;

int window_size = 800; // 윈도우 사이즈
float object_size = 0.35; // 찻주전자 사이즈

GLuint tableList; // 디스플레이 리스트 아이디

// 테이블 정점 좌표
GLfloat TableVertices[8][3] = {
	{-0.75,-0.4, 0.45},
	{-0.75, -0.275,0.45},
	{0.75, -0.275, 0.45},
	{0.75,-0.4, 0.45},
	{-0.75,-0.4,-0.45},
	{-0.75, -0.275,-0.45},
	{0.75, -0.275,-0.45},
	{0.75,-0.4,-0.45}
};
// 테이블 각 정점의 색 - 표현이 안됨
GLfloat TableColors[] = {
	0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, 0.5, 0.5
};
// 테이블 면
GLubyte TableVertexList[24] = {
	0,3,2,1,
	2,3,7,6,
	0,4,7,3,
	1,2,6,5,
	4,5,6,7,
	0,1,5,4
};


// 테이블 다리 정점 좌표
GLfloat TableLegVertices[8][3] = {
	{-0.05, -0.9,  0.05},
	{-0.05, -0.275, 0.05},
	{0.05,  -0.275, 0.05},
	{0.05,  -0.9,   0.05},
	{-0.05, -0.9,  -0.05},
	{-0.05, -0.275, -0.05},
	{0.05,  -0.275, -0.05},
	{0.05,  -0.9,  -0.05}
};
// 테이블 다리 색 - 마찬가지로 표현이 안됨
GLfloat TableLegColors[] = {
	0.4, 0.4, 0.4,
	0.4, 0.4, 0.4,
	0.4, 0.4, 0.4,
	0.4, 0.4, 0.4,
	0.4, 0.4, 0.4,
	0.4, 0.4, 0.4
};
// 테이블 다리 면
GLubyte TableLegVertexList[24] = {
	0,3,2,1,
	2,3,7,6,
	0,4,7,3,
	1,2,6,5,
	4,5,6,7,
	0,1,5,4
};

// 빛 설정?
void InitLight() {
	GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_shininess[] = { 15.0 };

	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_position[] = { 3, 6, 3.0, 1.0 };

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

}

// 키보드 q, s, w 입력 이벤트
void MyKeyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'q': case'Q': case '\033': // 종료
		exit(0);
		break;
	case 's': // 부드럽게
		if (FlatShaded) {
			FlatShaded = 0;
			glShadeModel(GL_SMOOTH);
		}
		else {
			FlatShaded = 1;
			glShadeModel(GL_FLAT);
		}
		glutPostRedisplay();
		break;

	// 추가 부분 - 와이어 프레임 토글링 코드를 삽입하라 전역변수 Wireframed를 사용 - glPolygonMode를 사용하면 된다
	case 'w':
		if (Wireframed) {
			Wireframed = 0;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			Wireframed = 1;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		glutPostRedisplay();
		break;

	case 'e':
		if (ViewMode > 0) {
			ViewMode = -1;
		}
		else {
			ViewMode = 1;
		}
		vX = 0;
		vY = 0;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}


// 마우스 이동 이벤트 좌표를 변수에 저장
void MyMouseMove(GLint X, GLint Y) {
	// 추가 부분 - 마우스 움직임 X, Y를 전역변수 View X, Y에 할당하라
	ViewX = vX + X - (window_size / 2);
	ViewY = vY + Y - (window_size / 2);

	glutPostRedisplay();
}

// 마우스 좌클릭 or 우클릭 
void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
		
	}
	else if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP) {
		vX = ViewX;
		vY = ViewY;
	}
}



// 디스플레이 리스트 - 테이블
void CreateTable() {
    tableList = glGenLists(1); // 디스플레이 리스트 생성
    glNewList(tableList, GL_COMPILE); // 리스트 컴파일 시작

	glFrontFace(GL_CCW); // 반시계로 나열
	//glEnable(GL_CULL_FACE); // 가려지는 면을 안보이게 설정

	/* 테이블 몸통 그리기 */
	glEnableClientState(GL_COLOR_ARRAY); // 정점 배열 기능 활성화 (정검, 컬러, 법선 벡터 등...)
	glEnableClientState(GL_VERTEX_ARRAY);

	glColorPointer(3, GL_FLOAT, 0, TableColors); // 정점에 대한 배열을 입력 
	glVertexPointer(3, GL_FLOAT, 0, TableVertices);

	for (GLint i = 0; i < 6; i++) {
		// POLYGON은 다각형을 의미, 4는 4개 인덱스로 구성됨, 타입 지정, 시작 주소를 변경 0, 4, 8, ...
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, &TableVertexList[4 * i]);
	}

	/* 다리 그리기 - 하나의 다리를 기준으로 각 테이블 모서리 위치쪽으로 4개를 그리게 함 */
	float x = 0.65;
	float y = 0.35;
	GLfloat offsetX[4] = { -x, x, -x, x }; // X
	GLfloat offsetZ[4] = { y, y, -y, -y }; // Z

	glColorPointer(3, GL_FLOAT, 0, TableLegColors);
	glVertexPointer(3, GL_FLOAT, 0, TableLegVertices);

	for (GLint i = 0; i < 4; i++) {
		glPushMatrix(); // 현재 행렬 저장 - 값이 바뀌어서 원본을 저장하고, 바꾼 다음 그리고, 이후 다시 원본으로 복구
		glTranslatef(offsetX[i], 0.0f, offsetZ[i]); // 각 다리를 테이블 모서리쪽으로 이동 (좌표값이 바뀜 - 저장을 해야한다)

		for (GLint j = 0; j < 6; j++)
			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, &TableLegVertexList[4 * j]);

		glPopMatrix(); // 변경한 행렬을 다시 복구
	}

    glEndList(); // 리스트 컴파일 종료
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float x = ViewX / (window_size / 2.0);
	float y = ViewY / (window_size / 2.0);

	// 추가 부분 - 마우스 움직임에 따라 변화
	gluLookAt(
		0, 0, 0,
		x, y, ViewMode,
		0, 1, 0
	);

	// 좌표 프린트
	printf("x : %.3f \t y : %.3f \t mode : %d\n", x, y, ViewMode);


	// 찻주전자의 재질 설정 (예시: 빨간색 재질)
	GLfloat teapot_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // 빨강
	glMaterialfv(GL_FRONT, GL_DIFFUSE, teapot_diffuse);
	glutSolidTeapot(object_size); // 찻주전자 가운데

	// 테이블의 재질 설정 (회색 재질)
	GLfloat table_diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 회색
	glMaterialfv(GL_FRONT, GL_DIFFUSE, table_diffuse);
	glCallList(tableList); // 테이블

    glFlush();
}

void MyReshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(window_size, window_size);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("HW OpenGL");
	glClearColor(0.4, 0.4, 0.4, 0.0);
	InitLight();

	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard); // 키 입력에 따라 렌더링 방식 변경
	glutMotionFunc(MyMouseMove); // 클릭 후 이동하면 오브젝트 시점 변경
	glutMouseFunc(MyMouseClick); // 마우스 클릭 좌클릭 사이즈 업, 우클릭 사이즈 다운
	glutReshapeFunc(MyReshape);

    CreateTable();

	glutMainLoop();

	return 0;
}