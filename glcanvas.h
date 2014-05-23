#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <QGLWidget>
#include <QVector>
#include <QMap>

typedef struct
{
  GLfloat x;
  GLfloat y;
  GLfloat z;

  GLfloat r;
  GLfloat g;
  GLfloat b;
} Point;

typedef struct
{
  float index1;
  float index2;
} OptimalColor;

class GLCanvas : public QGLWidget
{
public:
  explicit GLCanvas(QWidget *parent = 0);

protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

  void keyPressEvent(QKeyEvent*);

private:
  void computeType1Points();
  void computeType2Points();

  QVector <Point> _pointMountains;
  QVector <Point> _pointValleys;
  QVector <OptimalColor> _optimalColors;

  float _K;
  int _N;
  const float _delta_Y;
  const float _Y;
  const float _delta_lambda;


  GLuint _vBuffer[2];

  float* _y_barSum;

  float _eyeX;
  float _eyeY;
  float _eyeZ;

};

#endif // GLCANVAS_H
