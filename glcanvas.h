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

private:
  void computeType1Points();
  int getYBar (float lambda, float *y_bar);

  QVector <Point> _points;
  QVector <OptimalColor> _optimalColors;

  float _K;
  int _N;
  const float _delta_Y;
  const float _delta_lambda;
  const float _Y;
  QMap <float, float> _y_barValuesFromLambda;

};

#endif // GLCANVAS_H
