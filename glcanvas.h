#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <QGLWidget>
#include <QVector>

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

private:
  void computeType1Points();

  QVector <Point> _points;
  QVector <OptimalColor> _optimalColors;

  float _K;
  const int _N;
  const float _delta_Y;
  const float _delta_lambda;
  const float _Y;
};

#endif // GLCANVAS_H
