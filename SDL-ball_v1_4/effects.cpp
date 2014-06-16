/* ************************************************************************* *
    SDL-Ball - DX-Ball/Breakout remake with openGL and SDL for Linux
    Copyright (C) 2008 Jimmy Christensen ( dusted at dusted dot dk )

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************* */

struct effect_vars {
  int type;
  bool coldet;
  bool active;
  GLfloat col[3];
  GLfloat speed;
  GLfloat spread;
  GLfloat size;
  struct pos rect;  //rectangular size (for particleField)
  GLfloat gravity;
  int num;
  int life;
  textureClass tex; //Texture
  int effectId; //unique id for this effect
};

class transit_effect_class {
  private:
  GLfloat opacity;

  public:
  int age;
  effect_vars vars;
  void init()
  {
    age=0;
    opacity = 0.0f;
    var.transition_half_done=0;

  }

  void draw()
  {
    age += globalTicksSinceLastDraw;

    if(age < vars.life/2.0)
    {

      opacity += vars.life/500.0*globalMilliTicksSinceLastDraw;
    } else {
      var.transition_half_done=1;

      opacity -= vars.life/500.0*globalMilliTicksSinceLastDraw;
    }

    glLoadIdentity();
    glDisable( GL_TEXTURE_2D );
    glTranslatef(0.0, 0.0, -3.0);
    glColor4f( vars.col[0], vars.col[1], vars.col[2], opacity );
    glBegin( GL_QUADS );
      glVertex3f(-1.66, 1.25, 0.0);
      glVertex3f( 1.66, 1.25, 0.0);
      glVertex3f( 1.66,-1.25, 0.0);
      glVertex3f(-1.66,-1.25, 0.0);
    glEnd( );
    glEnable( GL_TEXTURE_2D );
  }

};

class sparkle {
  public:
  bool active;
  GLfloat size;
  GLfloat ang;
  int life;
  int lifeleft;
  pos p,v;
  effect_vars vars;
  GLfloat bounce,f;
  sparkle()
  {
    bounce=0;
    f=0;
    active=1;
  }


  void draw()
  {
    if(lifeleft > 0)
    {
      //sanitize
      lifeleft -= globalTicksSinceLastDraw;

      if(p.x > 1.67 || p.y < -1.7 || p.x < -1.67)
      {
        active=0;
      }

      v.y -= vars.gravity*globalMilliTicksSinceLastDraw;
      v.y -= bounce*globalMilliTicksSinceLastDraw;

      if(v.x < 0)
      {
        v.x += f*globalMilliTicksSinceLastDraw;
      } else {
        v.x -= f*globalMilliTicksSinceLastDraw;
      }

      p.x += v.x * globalMilliTicksSinceLastDraw;
      p.y += v.y * globalMilliTicksSinceLastDraw;





      glColor4f(vars.col[0], vars.col[1],vars.col[2], (1.0/(float)life)*(float)lifeleft);

    GLfloat curSize = size/(float)life * (float)lifeleft;

    glBindTexture( GL_TEXTURE_2D, vars.tex.prop.texture );
    glBegin( GL_QUADS );
      glTexCoord2f(0,0); glVertex3f(p.x-curSize,p.y+curSize, 0.0);
      glTexCoord2f(0,1); glVertex3f(p.x+curSize,p.y+curSize, 0.0);
      glTexCoord2f(1,1); glVertex3f(p.x+curSize,p.y-curSize, 0.0);
      glTexCoord2f(1,0); glVertex3f(p.x-curSize,p.y-curSize, 0.0);
    glEnd( );

    } else {
      active=0;
    }

  }

  void coldet(brick & b)
  {
    if(p.x > b.posx-b.width && p.x < b.posx+b.width)
    {
      if(p.y > b.posy-b.height && p.y < b.posy+b.height)
      {

        if(p.y < b.posy && !b.n(3))
        {

          v.y *= -1;
          //p.y = b.posy-b.height-0.01;
        } else

        if(p.y > b.posy && !b.n(2))
        {

          p.y = b.posy+b.height;
          if(v.y < 0)
          {
            v.y *= -1;
            v.y /= 2.0;
            f += 0.01;
          }
        } else

        if(p.x > b.posx && !b.n(1))
        {
          //p.x = b.posx+b.width;

          if(v.x < 0)
            v.x *= -1;
        } else

        if(p.x < b.posx && !b.n(0))
        {
          //p.x = b.posx-b.width;

          if(v.x > 0)
            v.x *= -1;
        }
      }
    }
  }

  void pcoldet(paddle_class & b)
  {
    if(p.x > b.posx-b.width && p.x < b.posx+b.width)
    {
      if(p.y > b.posy-b.height && p.y < b.posy+b.height)
      {


        if(p.y < b.posy)
        {

          p.y = b.posy-b.height;
        }

        if(p.y > b.posy)
        {

          p.y = b.posy+b.height;
          if(v.y < 0)
          {
            v.y *= -1;
            v.y /= 2.0;
            f += 0.01;
          }
        } else

        if(p.x > b.posx)
        {
          p.x = b.posx+b.width;

          if(v.x < 0)
            v.x *= -1;
        } else

        if(p.x < b.posx)
        {
          p.x = b.posx-b.width;

          if(v.x > 0)
            v.x *= -1;
        }
      }
    }
  }
};

class particleFieldClass {
  private:
    int spawnTimeout; //This is used to spawn sparkles at a random time
    sparkle *sparks;
    void spawnSpark(int sparkNum);
  public:
    struct pos p;
    struct effect_vars vars;
    void init(struct effect_vars varsP, struct pos p);
    void draw();
    void move(struct pos p);
    void coldet(brick & b);
    void pcoldet(paddle_class & b);
    ~particleFieldClass();
};

void particleFieldClass::init(struct effect_vars varsP, struct pos spawnPos)
{
  vars=varsP;
  spawnTimeout=0;
  vars.active=1;
  sparks = new sparkle[vars.num];
  p=spawnPos;

  for(int i=0; i < vars.num; i++)
  {
    sparks[i].active=1;
    sparks[i].vars = vars; //NOTE:remove effect_vars from sparkle class?
    spawnSpark(i);
  }
}

particleFieldClass::~particleFieldClass()
{
  delete[] sparks; //free the sparks
}

void particleFieldClass::spawnSpark(int sparkNum)
{
  GLfloat angle=(RAD/vars.num-1)*(rndflt(vars.num,0.0)); //FIXME: Make this elegant: Choose a random angle (in radients)
  spawnTimeout = rand()%10;
  sparks[sparkNum].life = rand()%vars.life;
  sparks[sparkNum].lifeleft = sparks[sparkNum].life;
  sparks[sparkNum].v.x = (vars.speed*rndflt(vars.speed*2.0,0.0)) * sin(angle);
  sparks[sparkNum].v.y = (vars.speed*rndflt(vars.speed*2.0,0.0)) * cos(angle);
  sparks[sparkNum].size = rndflt(vars.size,0);

  sparks[sparkNum].p=p;

  sparks[sparkNum].p.x += rndflt(vars.rect.x, (vars.rect.x/2.0));
  sparks[sparkNum].p.y += rndflt(vars.rect.y, (vars.rect.y/2.0));
  sparks[sparkNum].active=1;
}

void particleFieldClass::draw()
{
  spawnTimeout -= globalTicksSinceLastDraw;

  glLoadIdentity();
  glTranslatef(0.0,0.0 , -3.0);
  int t=0;

  for(int i=0; i<vars.num; i++)
  {
    if(sparks[i].active)
    {
      t++;
      sparks[i].draw();
    }
  }

  vars.life -=globalTicksSinceLastDraw;
  if(t==0)
  {
    vars.active=0;
  }
}

void particleFieldClass::coldet(brick & b)
{
  for(int i=0; i < vars.num; i++)
  {
    if(sparks[i].active)
      sparks[i].coldet(b);
  }
}

void particleFieldClass::pcoldet(paddle_class & b)
{
  for(int i=0; i < vars.num; i++)
  {
    if(sparks[i].active)
      sparks[i].pcoldet(b);
  }
}

class effect_class {
  private:
    pos spawn_pos;
    sparkle* sparks;


  public:
    transit_effect_class transit;
    particleFieldClass *pf;
    struct effect_vars vars;

    effect_class()
    {
      vars.active=0;
    }

    void init(pos p)
    {
      int i;
      GLfloat angle=0.0;
      vars.active=1;
      spawn_pos=p;
      int life;
      //cout << "Spawned effect type " << vars.type << " at " << p.x << "," << p.y << " with " << vars.life << "ms life" << endl;

      switch(vars.type)
      {
        case FX_SPARKS:
          sparks = new sparkle[vars.num];
          if(sparks == NULL)
          {
            cout << "Could not allocate " << vars.num << " sparks" << endl;
            i=vars.num+10;
          }

          for(i=0; i < vars.num; i++)
          {
            sparks[i].size = rndflt(vars.size,0);
            life = rand()%vars.life;
            angle=(RAD/vars.num-1)*(rndflt(vars.num,0.0)); //FIXME: Make this elegant: Choose a random angle (in radients)
            sparks[i].life = life;
            sparks[i].lifeleft = life;
            sparks[i].v.x = (vars.speed*rndflt(vars.speed*2.0,0.0)) * sin(angle);
            sparks[i].v.y = (vars.speed*rndflt(vars.speed*2.0,0.0)) * cos(angle);

            sparks[i].vars = vars;

            sparks[i].p = spawn_pos;


          }
          break;

        case FX_TRANSIT:
          transit.vars = vars;
          transit.init();
          break;

        case FX_PARTICLEFIELD:
          pf=NULL;
          pf = new particleFieldClass;
          pf->init(vars, p);
          break;

      }

    }

    void draw()
    {
      int i;
      bool stay=0;
      switch(vars.type)
      {
        case FX_SPARKS:
          glLoadIdentity();
          glTranslatef(0.0,0.0 , -3.0);
          for(i=0; i < vars.num; i++)
          {
            if(sparks[i].active)
            {
              sparks[i].draw();
              stay=1;
            }
          }
          break;

        case FX_TRANSIT:
          transit.draw();
          if(transit.age <= vars.life)
            stay=1;
          break;

        case FX_PARTICLEFIELD:
          pf->draw();
          if(pf->vars.active)
            stay=1;
          break;

      }

      if(var.effectnum != -1 && vars.type != FX_TRANSIT)
        stay=1;

      if(!stay)
      {
        vars.active=0;
        switch(vars.type)
        {
          case FX_SPARKS:
            delete[] sparks; //Free sparks again ;)
            break;

          case FX_PARTICLEFIELD:
            delete pf; //remove the particlefield
            break;
        }

      }
    }

    void coldet(brick & b)
    {
      int i;
      if(vars.type == FX_SPARKS)
      {
        for(i=0; i < vars.num; i++)
        {
          if(sparks[i].active)
          {
            sparks[i].coldet(b);
          }
        }
      } else if(vars.type == FX_PARTICLEFIELD)
      {
        pf->coldet(b);
      }
    }

    void pcoldet(paddle_class & b)
    {
      int i;
      if(vars.type == FX_SPARKS)
      {
        for(i=0; i < vars.num; i++)
        {
          if(sparks[i].active)
          {
            sparks[i].pcoldet(b);
          }
        }
      } else if(vars.type == FX_PARTICLEFIELD)
      {
        pf->pcoldet(b);
      }
    }
};

class effectManager {
  private:
  struct effect_vars vars;
  int effectId; //ever rising number of a spawned effect.

  public:
  list<effect_class>effects;

  effectManager()
  {
    effects.clear();
    effectId=0;
  }
  void set_test() // test the set values from set(int var, GLfloat val)  // check if the function works and the values that are passed are set
  {
      cout << "Start test set_test1" << endl;
    int var_test = FX_VAR_SPEED;
    GLfloat val_test = 25;
    int val_testint = 20;
    set(FX_VAR_SPEED, val_test); // set the test value
    if (vars.speed != val_test) // compare the test value with the set value
    {
       cout << "error in set(int, GLfloat) wrong value: "  << vars.speed << endl;
    }
    else
    {
        cout << "no error" << endl;
    }
    cout << "Stop test set_test1" << endl;
  }

  void set(int var, GLfloat val)
  {
    switch(var)
    {
      case FX_VAR_SPEED:
        vars.speed = val;
        break;
      case FX_VAR_SPREAD:
        vars.spread = val;
        break;
      case FX_VAR_SIZE:
        vars.size = val;
        break;
      case FX_VAR_GRAVITY:
        vars.gravity = val;
    }
  }
   void set_test2() // test the set values from set(int var, int val)  // check if the function works and the values that are passed are set
  {
      cout << "Start test set_test2" << endl;
    int var_test = FX_VAR_NUM;
    GLfloat val_test = 25;
    int val_testint = 20;
    set(var_test, val_testint); // set the test values
    if (vars.num != val_testint) // compare the set value with the original value
    {
       cout << "error in set(int, int) wrong value: "  << vars.num << endl;
    }
    else
    {
        cout << "no error" << endl;
    }
    cout << "Stop test set_test1" << endl;
  }

  void set(int var, int val)
  {
    switch(var)
    {
      case FX_VAR_NUM:
        vars.num=val;
        break;
      case FX_VAR_LIFE:
        vars.life=val;
        break;
      case FX_VAR_TYPE:
        vars.type=val;
        break;
      case FX_VAR_COLDET:
        vars.coldet=val;
        break;
    }

  }

  void set(int var, GLfloat r, GLfloat g, GLfloat b) // test the set values from set(int var, r, g, b)  // check if the function works and the values that are passed are set
  {
    switch(var)
    {
      case FX_VAR_COLOR:
        vars.col[0] = r;
        vars.col[1] = g;
        vars.col[2] = b;
        break;
    }
  }
   void set_test3()
  {      cout << "Start test set_test2" << endl;
    int var_test = FX_VAR_COLOR;
    GLfloat r = 1;
    GLfloat g = 2;
    GLfloat b = 3;
    set(var_test, r, g, b);
    if (vars.col[0] != r ) //check for each color if the values is set right by the function, else indicate for every different color an error
    {
       cout << "error in set(int, r, g, b) wrong r value: "  << vars.col[0] << endl;
    }
    if (vars.col[1] != g )
    {
       cout << "error in set(int, r, g, b) wrong g value: "  << vars.col[1] << endl;
    }
    if (vars.col[2] != b )
    {
       cout << "error in set(int, r, g, b) wrong b value: "  << vars.col[2] << endl;
    }
    if ( vars.col[2] == b && vars.col[2] == b && vars.col[2] == b)
    {
       cout << "no errors in set(int, r, g, b)" << endl;
    }
    cout << "Stop test set_test3" << endl;
  }

  void set(int var, textureClass tex)
  {
    switch(var)
    {
      case FX_VAR_TEXTURE:
        vars.tex = tex;
        break;
    }
  }
    void set_test4(textureClass tex) // comparison of object parameters, test if object is set right by set
  {
      cout << "Start test set_test4" << endl;
    int var_test = FX_VAR_TEXTURE;
    textureClass val_test = tex;
    set(var_test, val_test);
    int returnval_test = val_test.compare_test(tex); // compare all the different private parameters of the object with the original passed parameters. returns -1 if there is an error and 0 otherwise
    if (returnval_test == -1) // if there is an error, print tot stdout
    {
       cout << "error in set(int, textureClass) in object"<< endl;
    }
    else
    {
        cout << "no error" << endl;
    }
    cout << "Stop test set_test4" << endl;
  }

  void set(int var, struct pos p)
  {
    switch(var)
    {
      case FX_VAR_RECTANGLE:
        vars.rect = p;
        break;
    }
  }

  int spawn(pos p)
  {
    effectId++;
    effect_class tempEffect;
    vars.effectId = effectId;
    tempEffect.vars = vars;
    tempEffect.init(p);
    effects.push_back(tempEffect);

    return(effectId);
  }

  void draw()
  {
    for(list <effect_class>::iterator it = effects.begin(); it != effects.end(); ++it)
    {

      it->draw();

      if(!it->vars.active)
      {
        it = effects.erase(it);
      }
    }
  }

  void coldet(brick & b)
  {
    if(b.collide && b.active)
    {
      for(list <effect_class>::iterator it = effects.begin(); it != effects.end(); ++it)
      {
        if(it->vars.coldet)
          it->coldet(b);

      }
    }
  }

  void pcoldet(paddle_class & b)
  {

    for(list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it)
    {
      if(it->vars.coldet)
        it->pcoldet(b);
    }
  }

  int isActive(int id)
  {
    for(list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it)
    {
      if(it->vars.effectId == id && it->vars.active)
      {
        return(1);
      }
    }
    return(0);
  }

  void kill(int id)
  {
    for(list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it)
    {
      if(it->vars.effectId)
      {
        it->vars.active=0;
      }
    }
  }
};

class glAnnounceMessageClass {
  private:
    int age;
    GLfloat zoom,fade;
    bool fadedir;
  public:
    bool active;
    int life;
    string text;
    int font;

    glAnnounceMessageClass()
    {
      active=1;
      age=0;
      fade=0;
      fadedir=0;
      zoom=0;
    }

    void draw()
    {
        GLfloat s;
        zoom += 4000.0/life * globalMilliTicksSinceLastDraw;

        if(fadedir)
        {
          fade -= 4000.0/life * globalMilliTicksSinceLastDraw;
        } else {
          fade += 4000.0/life * globalMilliTicksSinceLastDraw;
        }

        glLoadIdentity();
        glTranslatef(0.0,0.0,-3.0);

        glColor4f(1.0,0.0,0.0,fade);
        s=zoom*0.85;
        glText->write(text, font, 1, s, 0.0, 0.0);

        glColor4f(0.0,1.0,0.0,fade);
        s=zoom*0.90;
        glText->write(text, font, 1, s, 0.0, 0.0);

        glColor4f(0.0,0.0,1.0,fade);
        s=zoom*0.95;
        glText->write(text, font, 1, s, 0.0, 0.0);

        glColor4f(1.0,1.0,1.0,fade);
        s=zoom;
        glText->write(text, font, 1, s, 0.0, 0.0);


        age += globalTicksSinceLastDraw;
        if(age > life*0.50)
        {
          fadedir=1;
        }

      if(age > life)
        active=0;
    }

};

#define MAXMSG 10
class glAnnounceTextClass {
  private:
    int len;
    list<glAnnounceMessageClass> msg;
    list<glAnnounceMessageClass>::iterator it;
  public:
    glAnnounceTextClass()
    {
      len=0;
    }

    void write(const char *text, int ttl, int font)
    {
      len++;

      msg.resize(len);

      it=msg.end();
      --it;

      it->life=ttl;
      it->text=text;
      it->font=font;
    }

    void draw()
    {
      if(len>0)
      {
        it=msg.begin();
        if(it->active)
        {
          it->draw();
        } else {

          msg.erase(it);
          len--;
        }
      }
    }

};
