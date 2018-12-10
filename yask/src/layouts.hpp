// Automatically generated; do not edit.
#include <stddef.h>

// 1-D <-> 1-D layout base class.
class Layout_1d {
protected:
  idx_t _d1 = 1;

public:

  Layout_1d() { }
  Layout_1d(idx_t d1) : _d1(d1) { }

  // Dimension 1.
  virtual idx_t get_d1() const { return _d1; };
  virtual void set_d1(idx_t d1) { _d1 = d1; };

  // Return overall number of elements.
  virtual idx_t get_size() const { return _d1; };

  // Return 1-D offset from 1-D 'j' indices.
  virtual idx_t layout(idx_t j1) const =0;

  // Set 1 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1) const =0;
};

// 1-D <-> 1-D layout class with dimensions in d1 order,
// meaning d1 is stored with unit stride.
class Layout_1 : public Layout_1d {
public:

  Layout_1() { }

  Layout_1(idx_t d1) : Layout_1d(d1) { }

  // Return 1-D offset from 1-D 'j' indices.
  virtual idx_t layout(idx_t j1) const final
    { return j1; }

  // set 1 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1) const final
    { j1 = (ai); }
};

// 2-D <-> 1-D layout base class.
class Layout_2d {
protected:
  idx_t _d1 = 1, _d2 = 1;

public:

  Layout_2d() { }
  Layout_2d(idx_t d1, idx_t d2) : _d1(d1), _d2(d2) { }

  // Dimension 1.
  virtual idx_t get_d1() const { return _d1; };
  virtual void set_d1(idx_t d1) { _d1 = d1; };

  // Dimension 2.
  virtual idx_t get_d2() const { return _d2; };
  virtual void set_d2(idx_t d2) { _d2 = d2; };

  // Return overall number of elements.
  virtual idx_t get_size() const { return _d1 * _d2; };

  // Return 1-D offset from 2-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2) const =0;

  // Set 2 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2) const =0;
};

// 2-D <-> 1-D layout class with dimensions in d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_12 : public Layout_2d {
public:

  Layout_12() { }

  Layout_12(idx_t d1, idx_t d2) : Layout_2d(d1, d2) { }

  // Return 1-D offset from 2-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2) const final
    { return j1 * _d2 + j2; }

  // set 2 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2) const final
    { j1 = (ai)/(_d2); j2 = (ai) % _d2; }
};

// 2-D <-> 1-D layout class with dimensions in d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_21 : public Layout_2d {
public:

  Layout_21() { }

  Layout_21(idx_t d1, idx_t d2) : Layout_2d(d1, d2) { }

  // Return 1-D offset from 2-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2) const final
    { return j2 * _d1 + j1; }

  // set 2 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2) const final
    { j2 = (ai)/(_d1); j1 = (ai) % _d1; }
};

// 3-D <-> 1-D layout base class.
class Layout_3d {
protected:
  idx_t _d1 = 1, _d2 = 1, _d3 = 1;

public:

  Layout_3d() { }
  Layout_3d(idx_t d1, idx_t d2, idx_t d3) : _d1(d1), _d2(d2), _d3(d3) { }

  // Dimension 1.
  virtual idx_t get_d1() const { return _d1; };
  virtual void set_d1(idx_t d1) { _d1 = d1; };

  // Dimension 2.
  virtual idx_t get_d2() const { return _d2; };
  virtual void set_d2(idx_t d2) { _d2 = d2; };

  // Dimension 3.
  virtual idx_t get_d3() const { return _d3; };
  virtual void set_d3(idx_t d3) { _d3 = d3; };

  // Return overall number of elements.
  virtual idx_t get_size() const { return _d1 * _d2 * _d3; };

  // Return 1-D offset from 3-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3) const =0;

  // Set 3 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3) const =0;
};

// 3-D <-> 1-D layout class with dimensions in d1, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_123 : public Layout_3d {
public:

  Layout_123() { }

  Layout_123(idx_t d1, idx_t d2, idx_t d3) : Layout_3d(d1, d2, d3) { }

  // Return 1-D offset from 3-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3) const final
    { return j1 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 3 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3) const final
    { j1 = (ai)/(_d2 * _d3); j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 3-D <-> 1-D layout class with dimensions in d1, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_132 : public Layout_3d {
public:

  Layout_132() { }

  Layout_132(idx_t d1, idx_t d2, idx_t d3) : Layout_3d(d1, d2, d3) { }

  // Return 1-D offset from 3-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3) const final
    { return j1 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 3 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3) const final
    { j1 = (ai)/(_d3 * _d2); j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 3-D <-> 1-D layout class with dimensions in d2, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_213 : public Layout_3d {
public:

  Layout_213() { }

  Layout_213(idx_t d1, idx_t d2, idx_t d3) : Layout_3d(d1, d2, d3) { }

  // Return 1-D offset from 3-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3) const final
    { return j2 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 3 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3) const final
    { j2 = (ai)/(_d1 * _d3); j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 3-D <-> 1-D layout class with dimensions in d2, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_231 : public Layout_3d {
public:

  Layout_231() { }

  Layout_231(idx_t d1, idx_t d2, idx_t d3) : Layout_3d(d1, d2, d3) { }

  // Return 1-D offset from 3-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3) const final
    { return j2 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 3 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3) const final
    { j2 = (ai)/(_d3 * _d1); j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 3-D <-> 1-D layout class with dimensions in d3, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_312 : public Layout_3d {
public:

  Layout_312() { }

  Layout_312(idx_t d1, idx_t d2, idx_t d3) : Layout_3d(d1, d2, d3) { }

  // Return 1-D offset from 3-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3) const final
    { return j3 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 3 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3) const final
    { j3 = (ai)/(_d1 * _d2); j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 3-D <-> 1-D layout class with dimensions in d3, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_321 : public Layout_3d {
public:

  Layout_321() { }

  Layout_321(idx_t d1, idx_t d2, idx_t d3) : Layout_3d(d1, d2, d3) { }

  // Return 1-D offset from 3-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3) const final
    { return j3 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 3 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3) const final
    { j3 = (ai)/(_d2 * _d1); j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 4-D <-> 1-D layout base class.
class Layout_4d {
protected:
  idx_t _d1 = 1, _d2 = 1, _d3 = 1, _d4 = 1;

public:

  Layout_4d() { }
  Layout_4d(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : _d1(d1), _d2(d2), _d3(d3), _d4(d4) { }

  // Dimension 1.
  virtual idx_t get_d1() const { return _d1; };
  virtual void set_d1(idx_t d1) { _d1 = d1; };

  // Dimension 2.
  virtual idx_t get_d2() const { return _d2; };
  virtual void set_d2(idx_t d2) { _d2 = d2; };

  // Dimension 3.
  virtual idx_t get_d3() const { return _d3; };
  virtual void set_d3(idx_t d3) { _d3 = d3; };

  // Dimension 4.
  virtual idx_t get_d4() const { return _d4; };
  virtual void set_d4(idx_t d4) { _d4 = d4; };

  // Return overall number of elements.
  virtual idx_t get_size() const { return _d1 * _d2 * _d3 * _d4; };

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const =0;

  // Set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const =0;
};

// 4-D <-> 1-D layout class with dimensions in d1, d2, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_1234 : public Layout_4d {
public:

  Layout_1234() { }

  Layout_1234(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j1 * _d2 * _d3 * _d4 + j2 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j1 = (ai)/(_d2 * _d3 * _d4); j2 = (ai)/(_d3 * _d4) % _d2; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 4-D <-> 1-D layout class with dimensions in d1, d2, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_1243 : public Layout_4d {
public:

  Layout_1243() { }

  Layout_1243(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j1 * _d2 * _d4 * _d3 + j2 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j1 = (ai)/(_d2 * _d4 * _d3); j2 = (ai)/(_d4 * _d3) % _d2; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 4-D <-> 1-D layout class with dimensions in d1, d3, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_1324 : public Layout_4d {
public:

  Layout_1324() { }

  Layout_1324(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j1 * _d3 * _d2 * _d4 + j3 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j1 = (ai)/(_d3 * _d2 * _d4); j3 = (ai)/(_d2 * _d4) % _d3; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 4-D <-> 1-D layout class with dimensions in d1, d3, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_1342 : public Layout_4d {
public:

  Layout_1342() { }

  Layout_1342(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j1 * _d3 * _d4 * _d2 + j3 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j1 = (ai)/(_d3 * _d4 * _d2); j3 = (ai)/(_d4 * _d2) % _d3; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 4-D <-> 1-D layout class with dimensions in d1, d4, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_1423 : public Layout_4d {
public:

  Layout_1423() { }

  Layout_1423(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j1 * _d4 * _d2 * _d3 + j4 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j1 = (ai)/(_d4 * _d2 * _d3); j4 = (ai)/(_d2 * _d3) % _d4; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 4-D <-> 1-D layout class with dimensions in d1, d4, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_1432 : public Layout_4d {
public:

  Layout_1432() { }

  Layout_1432(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j1 * _d4 * _d3 * _d2 + j4 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j1 = (ai)/(_d4 * _d3 * _d2); j4 = (ai)/(_d3 * _d2) % _d4; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 4-D <-> 1-D layout class with dimensions in d2, d1, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_2134 : public Layout_4d {
public:

  Layout_2134() { }

  Layout_2134(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j2 * _d1 * _d3 * _d4 + j1 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j2 = (ai)/(_d1 * _d3 * _d4); j1 = (ai)/(_d3 * _d4) % _d1; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 4-D <-> 1-D layout class with dimensions in d2, d1, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_2143 : public Layout_4d {
public:

  Layout_2143() { }

  Layout_2143(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j2 * _d1 * _d4 * _d3 + j1 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j2 = (ai)/(_d1 * _d4 * _d3); j1 = (ai)/(_d4 * _d3) % _d1; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 4-D <-> 1-D layout class with dimensions in d2, d3, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_2314 : public Layout_4d {
public:

  Layout_2314() { }

  Layout_2314(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j2 * _d3 * _d1 * _d4 + j3 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j2 = (ai)/(_d3 * _d1 * _d4); j3 = (ai)/(_d1 * _d4) % _d3; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 4-D <-> 1-D layout class with dimensions in d2, d3, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_2341 : public Layout_4d {
public:

  Layout_2341() { }

  Layout_2341(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j2 * _d3 * _d4 * _d1 + j3 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j2 = (ai)/(_d3 * _d4 * _d1); j3 = (ai)/(_d4 * _d1) % _d3; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 4-D <-> 1-D layout class with dimensions in d2, d4, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_2413 : public Layout_4d {
public:

  Layout_2413() { }

  Layout_2413(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j2 * _d4 * _d1 * _d3 + j4 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j2 = (ai)/(_d4 * _d1 * _d3); j4 = (ai)/(_d1 * _d3) % _d4; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 4-D <-> 1-D layout class with dimensions in d2, d4, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_2431 : public Layout_4d {
public:

  Layout_2431() { }

  Layout_2431(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j2 * _d4 * _d3 * _d1 + j4 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j2 = (ai)/(_d4 * _d3 * _d1); j4 = (ai)/(_d3 * _d1) % _d4; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 4-D <-> 1-D layout class with dimensions in d3, d1, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_3124 : public Layout_4d {
public:

  Layout_3124() { }

  Layout_3124(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j3 * _d1 * _d2 * _d4 + j1 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j3 = (ai)/(_d1 * _d2 * _d4); j1 = (ai)/(_d2 * _d4) % _d1; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 4-D <-> 1-D layout class with dimensions in d3, d1, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_3142 : public Layout_4d {
public:

  Layout_3142() { }

  Layout_3142(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j3 * _d1 * _d4 * _d2 + j1 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j3 = (ai)/(_d1 * _d4 * _d2); j1 = (ai)/(_d4 * _d2) % _d1; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 4-D <-> 1-D layout class with dimensions in d3, d2, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_3214 : public Layout_4d {
public:

  Layout_3214() { }

  Layout_3214(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j3 * _d2 * _d1 * _d4 + j2 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j3 = (ai)/(_d2 * _d1 * _d4); j2 = (ai)/(_d1 * _d4) % _d2; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 4-D <-> 1-D layout class with dimensions in d3, d2, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_3241 : public Layout_4d {
public:

  Layout_3241() { }

  Layout_3241(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j3 * _d2 * _d4 * _d1 + j2 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j3 = (ai)/(_d2 * _d4 * _d1); j2 = (ai)/(_d4 * _d1) % _d2; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 4-D <-> 1-D layout class with dimensions in d3, d4, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_3412 : public Layout_4d {
public:

  Layout_3412() { }

  Layout_3412(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j3 * _d4 * _d1 * _d2 + j4 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j3 = (ai)/(_d4 * _d1 * _d2); j4 = (ai)/(_d1 * _d2) % _d4; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 4-D <-> 1-D layout class with dimensions in d3, d4, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_3421 : public Layout_4d {
public:

  Layout_3421() { }

  Layout_3421(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j3 * _d4 * _d2 * _d1 + j4 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j3 = (ai)/(_d4 * _d2 * _d1); j4 = (ai)/(_d2 * _d1) % _d4; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 4-D <-> 1-D layout class with dimensions in d4, d1, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_4123 : public Layout_4d {
public:

  Layout_4123() { }

  Layout_4123(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j4 * _d1 * _d2 * _d3 + j1 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j4 = (ai)/(_d1 * _d2 * _d3); j1 = (ai)/(_d2 * _d3) % _d1; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 4-D <-> 1-D layout class with dimensions in d4, d1, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_4132 : public Layout_4d {
public:

  Layout_4132() { }

  Layout_4132(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j4 * _d1 * _d3 * _d2 + j1 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j4 = (ai)/(_d1 * _d3 * _d2); j1 = (ai)/(_d3 * _d2) % _d1; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 4-D <-> 1-D layout class with dimensions in d4, d2, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_4213 : public Layout_4d {
public:

  Layout_4213() { }

  Layout_4213(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j4 * _d2 * _d1 * _d3 + j2 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j4 = (ai)/(_d2 * _d1 * _d3); j2 = (ai)/(_d1 * _d3) % _d2; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 4-D <-> 1-D layout class with dimensions in d4, d2, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_4231 : public Layout_4d {
public:

  Layout_4231() { }

  Layout_4231(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j4 * _d2 * _d3 * _d1 + j2 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j4 = (ai)/(_d2 * _d3 * _d1); j2 = (ai)/(_d3 * _d1) % _d2; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 4-D <-> 1-D layout class with dimensions in d4, d3, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_4312 : public Layout_4d {
public:

  Layout_4312() { }

  Layout_4312(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j4 * _d3 * _d1 * _d2 + j3 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j4 = (ai)/(_d3 * _d1 * _d2); j3 = (ai)/(_d1 * _d2) % _d3; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 4-D <-> 1-D layout class with dimensions in d4, d3, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_4321 : public Layout_4d {
public:

  Layout_4321() { }

  Layout_4321(idx_t d1, idx_t d2, idx_t d3, idx_t d4) : Layout_4d(d1, d2, d3, d4) { }

  // Return 1-D offset from 4-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4) const final
    { return j4 * _d3 * _d2 * _d1 + j3 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 4 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4) const final
    { j4 = (ai)/(_d3 * _d2 * _d1); j3 = (ai)/(_d2 * _d1) % _d3; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout base class.
class Layout_5d {
protected:
  idx_t _d1 = 1, _d2 = 1, _d3 = 1, _d4 = 1, _d5 = 1;

public:

  Layout_5d() { }
  Layout_5d(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : _d1(d1), _d2(d2), _d3(d3), _d4(d4), _d5(d5) { }

  // Dimension 1.
  virtual idx_t get_d1() const { return _d1; };
  virtual void set_d1(idx_t d1) { _d1 = d1; };

  // Dimension 2.
  virtual idx_t get_d2() const { return _d2; };
  virtual void set_d2(idx_t d2) { _d2 = d2; };

  // Dimension 3.
  virtual idx_t get_d3() const { return _d3; };
  virtual void set_d3(idx_t d3) { _d3 = d3; };

  // Dimension 4.
  virtual idx_t get_d4() const { return _d4; };
  virtual void set_d4(idx_t d4) { _d4 = d4; };

  // Dimension 5.
  virtual idx_t get_d5() const { return _d5; };
  virtual void set_d5(idx_t d5) { _d5 = d5; };

  // Return overall number of elements.
  virtual idx_t get_size() const { return _d1 * _d2 * _d3 * _d4 * _d5; };

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const =0;

  // Set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const =0;
};

// 5-D <-> 1-D layout class with dimensions in d1, d2, d3, d4, d5 order,
// meaning d5 is stored with unit stride.
class Layout_12345 : public Layout_5d {
public:

  Layout_12345() { }

  Layout_12345(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d2 * _d3 * _d4 * _d5 + j2 * _d3 * _d4 * _d5 + j3 * _d4 * _d5 + j4 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d2 * _d3 * _d4 * _d5); j2 = (ai)/(_d3 * _d4 * _d5) % _d2; j3 = (ai)/(_d4 * _d5) % _d3; j4 = (ai)/(_d5) % _d4; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d2, d3, d5, d4 order,
// meaning d4 is stored with unit stride.
class Layout_12354 : public Layout_5d {
public:

  Layout_12354() { }

  Layout_12354(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d2 * _d3 * _d5 * _d4 + j2 * _d3 * _d5 * _d4 + j3 * _d5 * _d4 + j5 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d2 * _d3 * _d5 * _d4); j2 = (ai)/(_d3 * _d5 * _d4) % _d2; j3 = (ai)/(_d5 * _d4) % _d3; j5 = (ai)/(_d4) % _d5; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d2, d4, d3, d5 order,
// meaning d5 is stored with unit stride.
class Layout_12435 : public Layout_5d {
public:

  Layout_12435() { }

  Layout_12435(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d2 * _d4 * _d3 * _d5 + j2 * _d4 * _d3 * _d5 + j4 * _d3 * _d5 + j3 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d2 * _d4 * _d3 * _d5); j2 = (ai)/(_d4 * _d3 * _d5) % _d2; j4 = (ai)/(_d3 * _d5) % _d4; j3 = (ai)/(_d5) % _d3; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d2, d4, d5, d3 order,
// meaning d3 is stored with unit stride.
class Layout_12453 : public Layout_5d {
public:

  Layout_12453() { }

  Layout_12453(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d2 * _d4 * _d5 * _d3 + j2 * _d4 * _d5 * _d3 + j4 * _d5 * _d3 + j5 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d2 * _d4 * _d5 * _d3); j2 = (ai)/(_d4 * _d5 * _d3) % _d2; j4 = (ai)/(_d5 * _d3) % _d4; j5 = (ai)/(_d3) % _d5; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d2, d5, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_12534 : public Layout_5d {
public:

  Layout_12534() { }

  Layout_12534(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d2 * _d5 * _d3 * _d4 + j2 * _d5 * _d3 * _d4 + j5 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d2 * _d5 * _d3 * _d4); j2 = (ai)/(_d5 * _d3 * _d4) % _d2; j5 = (ai)/(_d3 * _d4) % _d5; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d2, d5, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_12543 : public Layout_5d {
public:

  Layout_12543() { }

  Layout_12543(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d2 * _d5 * _d4 * _d3 + j2 * _d5 * _d4 * _d3 + j5 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d2 * _d5 * _d4 * _d3); j2 = (ai)/(_d5 * _d4 * _d3) % _d2; j5 = (ai)/(_d4 * _d3) % _d5; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d3, d2, d4, d5 order,
// meaning d5 is stored with unit stride.
class Layout_13245 : public Layout_5d {
public:

  Layout_13245() { }

  Layout_13245(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d3 * _d2 * _d4 * _d5 + j3 * _d2 * _d4 * _d5 + j2 * _d4 * _d5 + j4 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d3 * _d2 * _d4 * _d5); j3 = (ai)/(_d2 * _d4 * _d5) % _d3; j2 = (ai)/(_d4 * _d5) % _d2; j4 = (ai)/(_d5) % _d4; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d3, d2, d5, d4 order,
// meaning d4 is stored with unit stride.
class Layout_13254 : public Layout_5d {
public:

  Layout_13254() { }

  Layout_13254(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d3 * _d2 * _d5 * _d4 + j3 * _d2 * _d5 * _d4 + j2 * _d5 * _d4 + j5 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d3 * _d2 * _d5 * _d4); j3 = (ai)/(_d2 * _d5 * _d4) % _d3; j2 = (ai)/(_d5 * _d4) % _d2; j5 = (ai)/(_d4) % _d5; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d3, d4, d2, d5 order,
// meaning d5 is stored with unit stride.
class Layout_13425 : public Layout_5d {
public:

  Layout_13425() { }

  Layout_13425(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d3 * _d4 * _d2 * _d5 + j3 * _d4 * _d2 * _d5 + j4 * _d2 * _d5 + j2 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d3 * _d4 * _d2 * _d5); j3 = (ai)/(_d4 * _d2 * _d5) % _d3; j4 = (ai)/(_d2 * _d5) % _d4; j2 = (ai)/(_d5) % _d2; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d3, d4, d5, d2 order,
// meaning d2 is stored with unit stride.
class Layout_13452 : public Layout_5d {
public:

  Layout_13452() { }

  Layout_13452(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d3 * _d4 * _d5 * _d2 + j3 * _d4 * _d5 * _d2 + j4 * _d5 * _d2 + j5 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d3 * _d4 * _d5 * _d2); j3 = (ai)/(_d4 * _d5 * _d2) % _d3; j4 = (ai)/(_d5 * _d2) % _d4; j5 = (ai)/(_d2) % _d5; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d3, d5, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_13524 : public Layout_5d {
public:

  Layout_13524() { }

  Layout_13524(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d3 * _d5 * _d2 * _d4 + j3 * _d5 * _d2 * _d4 + j5 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d3 * _d5 * _d2 * _d4); j3 = (ai)/(_d5 * _d2 * _d4) % _d3; j5 = (ai)/(_d2 * _d4) % _d5; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d3, d5, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_13542 : public Layout_5d {
public:

  Layout_13542() { }

  Layout_13542(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d3 * _d5 * _d4 * _d2 + j3 * _d5 * _d4 * _d2 + j5 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d3 * _d5 * _d4 * _d2); j3 = (ai)/(_d5 * _d4 * _d2) % _d3; j5 = (ai)/(_d4 * _d2) % _d5; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d4, d2, d3, d5 order,
// meaning d5 is stored with unit stride.
class Layout_14235 : public Layout_5d {
public:

  Layout_14235() { }

  Layout_14235(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d4 * _d2 * _d3 * _d5 + j4 * _d2 * _d3 * _d5 + j2 * _d3 * _d5 + j3 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d4 * _d2 * _d3 * _d5); j4 = (ai)/(_d2 * _d3 * _d5) % _d4; j2 = (ai)/(_d3 * _d5) % _d2; j3 = (ai)/(_d5) % _d3; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d4, d2, d5, d3 order,
// meaning d3 is stored with unit stride.
class Layout_14253 : public Layout_5d {
public:

  Layout_14253() { }

  Layout_14253(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d4 * _d2 * _d5 * _d3 + j4 * _d2 * _d5 * _d3 + j2 * _d5 * _d3 + j5 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d4 * _d2 * _d5 * _d3); j4 = (ai)/(_d2 * _d5 * _d3) % _d4; j2 = (ai)/(_d5 * _d3) % _d2; j5 = (ai)/(_d3) % _d5; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d4, d3, d2, d5 order,
// meaning d5 is stored with unit stride.
class Layout_14325 : public Layout_5d {
public:

  Layout_14325() { }

  Layout_14325(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d4 * _d3 * _d2 * _d5 + j4 * _d3 * _d2 * _d5 + j3 * _d2 * _d5 + j2 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d4 * _d3 * _d2 * _d5); j4 = (ai)/(_d3 * _d2 * _d5) % _d4; j3 = (ai)/(_d2 * _d5) % _d3; j2 = (ai)/(_d5) % _d2; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d4, d3, d5, d2 order,
// meaning d2 is stored with unit stride.
class Layout_14352 : public Layout_5d {
public:

  Layout_14352() { }

  Layout_14352(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d4 * _d3 * _d5 * _d2 + j4 * _d3 * _d5 * _d2 + j3 * _d5 * _d2 + j5 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d4 * _d3 * _d5 * _d2); j4 = (ai)/(_d3 * _d5 * _d2) % _d4; j3 = (ai)/(_d5 * _d2) % _d3; j5 = (ai)/(_d2) % _d5; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d4, d5, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_14523 : public Layout_5d {
public:

  Layout_14523() { }

  Layout_14523(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d4 * _d5 * _d2 * _d3 + j4 * _d5 * _d2 * _d3 + j5 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d4 * _d5 * _d2 * _d3); j4 = (ai)/(_d5 * _d2 * _d3) % _d4; j5 = (ai)/(_d2 * _d3) % _d5; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d4, d5, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_14532 : public Layout_5d {
public:

  Layout_14532() { }

  Layout_14532(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d4 * _d5 * _d3 * _d2 + j4 * _d5 * _d3 * _d2 + j5 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d4 * _d5 * _d3 * _d2); j4 = (ai)/(_d5 * _d3 * _d2) % _d4; j5 = (ai)/(_d3 * _d2) % _d5; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d5, d2, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_15234 : public Layout_5d {
public:

  Layout_15234() { }

  Layout_15234(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d5 * _d2 * _d3 * _d4 + j5 * _d2 * _d3 * _d4 + j2 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d5 * _d2 * _d3 * _d4); j5 = (ai)/(_d2 * _d3 * _d4) % _d5; j2 = (ai)/(_d3 * _d4) % _d2; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d5, d2, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_15243 : public Layout_5d {
public:

  Layout_15243() { }

  Layout_15243(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d5 * _d2 * _d4 * _d3 + j5 * _d2 * _d4 * _d3 + j2 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d5 * _d2 * _d4 * _d3); j5 = (ai)/(_d2 * _d4 * _d3) % _d5; j2 = (ai)/(_d4 * _d3) % _d2; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d5, d3, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_15324 : public Layout_5d {
public:

  Layout_15324() { }

  Layout_15324(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d5 * _d3 * _d2 * _d4 + j5 * _d3 * _d2 * _d4 + j3 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d5 * _d3 * _d2 * _d4); j5 = (ai)/(_d3 * _d2 * _d4) % _d5; j3 = (ai)/(_d2 * _d4) % _d3; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d5, d3, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_15342 : public Layout_5d {
public:

  Layout_15342() { }

  Layout_15342(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d5 * _d3 * _d4 * _d2 + j5 * _d3 * _d4 * _d2 + j3 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d5 * _d3 * _d4 * _d2); j5 = (ai)/(_d3 * _d4 * _d2) % _d5; j3 = (ai)/(_d4 * _d2) % _d3; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d5, d4, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_15423 : public Layout_5d {
public:

  Layout_15423() { }

  Layout_15423(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d5 * _d4 * _d2 * _d3 + j5 * _d4 * _d2 * _d3 + j4 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d5 * _d4 * _d2 * _d3); j5 = (ai)/(_d4 * _d2 * _d3) % _d5; j4 = (ai)/(_d2 * _d3) % _d4; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d1, d5, d4, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_15432 : public Layout_5d {
public:

  Layout_15432() { }

  Layout_15432(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j1 * _d5 * _d4 * _d3 * _d2 + j5 * _d4 * _d3 * _d2 + j4 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j1 = (ai)/(_d5 * _d4 * _d3 * _d2); j5 = (ai)/(_d4 * _d3 * _d2) % _d5; j4 = (ai)/(_d3 * _d2) % _d4; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d1, d3, d4, d5 order,
// meaning d5 is stored with unit stride.
class Layout_21345 : public Layout_5d {
public:

  Layout_21345() { }

  Layout_21345(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d1 * _d3 * _d4 * _d5 + j1 * _d3 * _d4 * _d5 + j3 * _d4 * _d5 + j4 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d1 * _d3 * _d4 * _d5); j1 = (ai)/(_d3 * _d4 * _d5) % _d1; j3 = (ai)/(_d4 * _d5) % _d3; j4 = (ai)/(_d5) % _d4; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d1, d3, d5, d4 order,
// meaning d4 is stored with unit stride.
class Layout_21354 : public Layout_5d {
public:

  Layout_21354() { }

  Layout_21354(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d1 * _d3 * _d5 * _d4 + j1 * _d3 * _d5 * _d4 + j3 * _d5 * _d4 + j5 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d1 * _d3 * _d5 * _d4); j1 = (ai)/(_d3 * _d5 * _d4) % _d1; j3 = (ai)/(_d5 * _d4) % _d3; j5 = (ai)/(_d4) % _d5; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d1, d4, d3, d5 order,
// meaning d5 is stored with unit stride.
class Layout_21435 : public Layout_5d {
public:

  Layout_21435() { }

  Layout_21435(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d1 * _d4 * _d3 * _d5 + j1 * _d4 * _d3 * _d5 + j4 * _d3 * _d5 + j3 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d1 * _d4 * _d3 * _d5); j1 = (ai)/(_d4 * _d3 * _d5) % _d1; j4 = (ai)/(_d3 * _d5) % _d4; j3 = (ai)/(_d5) % _d3; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d1, d4, d5, d3 order,
// meaning d3 is stored with unit stride.
class Layout_21453 : public Layout_5d {
public:

  Layout_21453() { }

  Layout_21453(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d1 * _d4 * _d5 * _d3 + j1 * _d4 * _d5 * _d3 + j4 * _d5 * _d3 + j5 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d1 * _d4 * _d5 * _d3); j1 = (ai)/(_d4 * _d5 * _d3) % _d1; j4 = (ai)/(_d5 * _d3) % _d4; j5 = (ai)/(_d3) % _d5; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d1, d5, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_21534 : public Layout_5d {
public:

  Layout_21534() { }

  Layout_21534(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d1 * _d5 * _d3 * _d4 + j1 * _d5 * _d3 * _d4 + j5 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d1 * _d5 * _d3 * _d4); j1 = (ai)/(_d5 * _d3 * _d4) % _d1; j5 = (ai)/(_d3 * _d4) % _d5; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d1, d5, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_21543 : public Layout_5d {
public:

  Layout_21543() { }

  Layout_21543(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d1 * _d5 * _d4 * _d3 + j1 * _d5 * _d4 * _d3 + j5 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d1 * _d5 * _d4 * _d3); j1 = (ai)/(_d5 * _d4 * _d3) % _d1; j5 = (ai)/(_d4 * _d3) % _d5; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d3, d1, d4, d5 order,
// meaning d5 is stored with unit stride.
class Layout_23145 : public Layout_5d {
public:

  Layout_23145() { }

  Layout_23145(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d3 * _d1 * _d4 * _d5 + j3 * _d1 * _d4 * _d5 + j1 * _d4 * _d5 + j4 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d3 * _d1 * _d4 * _d5); j3 = (ai)/(_d1 * _d4 * _d5) % _d3; j1 = (ai)/(_d4 * _d5) % _d1; j4 = (ai)/(_d5) % _d4; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d3, d1, d5, d4 order,
// meaning d4 is stored with unit stride.
class Layout_23154 : public Layout_5d {
public:

  Layout_23154() { }

  Layout_23154(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d3 * _d1 * _d5 * _d4 + j3 * _d1 * _d5 * _d4 + j1 * _d5 * _d4 + j5 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d3 * _d1 * _d5 * _d4); j3 = (ai)/(_d1 * _d5 * _d4) % _d3; j1 = (ai)/(_d5 * _d4) % _d1; j5 = (ai)/(_d4) % _d5; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d3, d4, d1, d5 order,
// meaning d5 is stored with unit stride.
class Layout_23415 : public Layout_5d {
public:

  Layout_23415() { }

  Layout_23415(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d3 * _d4 * _d1 * _d5 + j3 * _d4 * _d1 * _d5 + j4 * _d1 * _d5 + j1 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d3 * _d4 * _d1 * _d5); j3 = (ai)/(_d4 * _d1 * _d5) % _d3; j4 = (ai)/(_d1 * _d5) % _d4; j1 = (ai)/(_d5) % _d1; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d3, d4, d5, d1 order,
// meaning d1 is stored with unit stride.
class Layout_23451 : public Layout_5d {
public:

  Layout_23451() { }

  Layout_23451(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d3 * _d4 * _d5 * _d1 + j3 * _d4 * _d5 * _d1 + j4 * _d5 * _d1 + j5 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d3 * _d4 * _d5 * _d1); j3 = (ai)/(_d4 * _d5 * _d1) % _d3; j4 = (ai)/(_d5 * _d1) % _d4; j5 = (ai)/(_d1) % _d5; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d3, d5, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_23514 : public Layout_5d {
public:

  Layout_23514() { }

  Layout_23514(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d3 * _d5 * _d1 * _d4 + j3 * _d5 * _d1 * _d4 + j5 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d3 * _d5 * _d1 * _d4); j3 = (ai)/(_d5 * _d1 * _d4) % _d3; j5 = (ai)/(_d1 * _d4) % _d5; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d3, d5, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_23541 : public Layout_5d {
public:

  Layout_23541() { }

  Layout_23541(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d3 * _d5 * _d4 * _d1 + j3 * _d5 * _d4 * _d1 + j5 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d3 * _d5 * _d4 * _d1); j3 = (ai)/(_d5 * _d4 * _d1) % _d3; j5 = (ai)/(_d4 * _d1) % _d5; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d4, d1, d3, d5 order,
// meaning d5 is stored with unit stride.
class Layout_24135 : public Layout_5d {
public:

  Layout_24135() { }

  Layout_24135(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d4 * _d1 * _d3 * _d5 + j4 * _d1 * _d3 * _d5 + j1 * _d3 * _d5 + j3 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d4 * _d1 * _d3 * _d5); j4 = (ai)/(_d1 * _d3 * _d5) % _d4; j1 = (ai)/(_d3 * _d5) % _d1; j3 = (ai)/(_d5) % _d3; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d4, d1, d5, d3 order,
// meaning d3 is stored with unit stride.
class Layout_24153 : public Layout_5d {
public:

  Layout_24153() { }

  Layout_24153(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d4 * _d1 * _d5 * _d3 + j4 * _d1 * _d5 * _d3 + j1 * _d5 * _d3 + j5 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d4 * _d1 * _d5 * _d3); j4 = (ai)/(_d1 * _d5 * _d3) % _d4; j1 = (ai)/(_d5 * _d3) % _d1; j5 = (ai)/(_d3) % _d5; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d4, d3, d1, d5 order,
// meaning d5 is stored with unit stride.
class Layout_24315 : public Layout_5d {
public:

  Layout_24315() { }

  Layout_24315(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d4 * _d3 * _d1 * _d5 + j4 * _d3 * _d1 * _d5 + j3 * _d1 * _d5 + j1 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d4 * _d3 * _d1 * _d5); j4 = (ai)/(_d3 * _d1 * _d5) % _d4; j3 = (ai)/(_d1 * _d5) % _d3; j1 = (ai)/(_d5) % _d1; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d4, d3, d5, d1 order,
// meaning d1 is stored with unit stride.
class Layout_24351 : public Layout_5d {
public:

  Layout_24351() { }

  Layout_24351(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d4 * _d3 * _d5 * _d1 + j4 * _d3 * _d5 * _d1 + j3 * _d5 * _d1 + j5 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d4 * _d3 * _d5 * _d1); j4 = (ai)/(_d3 * _d5 * _d1) % _d4; j3 = (ai)/(_d5 * _d1) % _d3; j5 = (ai)/(_d1) % _d5; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d4, d5, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_24513 : public Layout_5d {
public:

  Layout_24513() { }

  Layout_24513(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d4 * _d5 * _d1 * _d3 + j4 * _d5 * _d1 * _d3 + j5 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d4 * _d5 * _d1 * _d3); j4 = (ai)/(_d5 * _d1 * _d3) % _d4; j5 = (ai)/(_d1 * _d3) % _d5; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d4, d5, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_24531 : public Layout_5d {
public:

  Layout_24531() { }

  Layout_24531(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d4 * _d5 * _d3 * _d1 + j4 * _d5 * _d3 * _d1 + j5 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d4 * _d5 * _d3 * _d1); j4 = (ai)/(_d5 * _d3 * _d1) % _d4; j5 = (ai)/(_d3 * _d1) % _d5; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d5, d1, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_25134 : public Layout_5d {
public:

  Layout_25134() { }

  Layout_25134(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d5 * _d1 * _d3 * _d4 + j5 * _d1 * _d3 * _d4 + j1 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d5 * _d1 * _d3 * _d4); j5 = (ai)/(_d1 * _d3 * _d4) % _d5; j1 = (ai)/(_d3 * _d4) % _d1; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d5, d1, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_25143 : public Layout_5d {
public:

  Layout_25143() { }

  Layout_25143(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d5 * _d1 * _d4 * _d3 + j5 * _d1 * _d4 * _d3 + j1 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d5 * _d1 * _d4 * _d3); j5 = (ai)/(_d1 * _d4 * _d3) % _d5; j1 = (ai)/(_d4 * _d3) % _d1; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d5, d3, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_25314 : public Layout_5d {
public:

  Layout_25314() { }

  Layout_25314(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d5 * _d3 * _d1 * _d4 + j5 * _d3 * _d1 * _d4 + j3 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d5 * _d3 * _d1 * _d4); j5 = (ai)/(_d3 * _d1 * _d4) % _d5; j3 = (ai)/(_d1 * _d4) % _d3; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d5, d3, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_25341 : public Layout_5d {
public:

  Layout_25341() { }

  Layout_25341(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d5 * _d3 * _d4 * _d1 + j5 * _d3 * _d4 * _d1 + j3 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d5 * _d3 * _d4 * _d1); j5 = (ai)/(_d3 * _d4 * _d1) % _d5; j3 = (ai)/(_d4 * _d1) % _d3; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d5, d4, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_25413 : public Layout_5d {
public:

  Layout_25413() { }

  Layout_25413(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d5 * _d4 * _d1 * _d3 + j5 * _d4 * _d1 * _d3 + j4 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d5 * _d4 * _d1 * _d3); j5 = (ai)/(_d4 * _d1 * _d3) % _d5; j4 = (ai)/(_d1 * _d3) % _d4; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d2, d5, d4, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_25431 : public Layout_5d {
public:

  Layout_25431() { }

  Layout_25431(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j2 * _d5 * _d4 * _d3 * _d1 + j5 * _d4 * _d3 * _d1 + j4 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j2 = (ai)/(_d5 * _d4 * _d3 * _d1); j5 = (ai)/(_d4 * _d3 * _d1) % _d5; j4 = (ai)/(_d3 * _d1) % _d4; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d1, d2, d4, d5 order,
// meaning d5 is stored with unit stride.
class Layout_31245 : public Layout_5d {
public:

  Layout_31245() { }

  Layout_31245(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d1 * _d2 * _d4 * _d5 + j1 * _d2 * _d4 * _d5 + j2 * _d4 * _d5 + j4 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d1 * _d2 * _d4 * _d5); j1 = (ai)/(_d2 * _d4 * _d5) % _d1; j2 = (ai)/(_d4 * _d5) % _d2; j4 = (ai)/(_d5) % _d4; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d1, d2, d5, d4 order,
// meaning d4 is stored with unit stride.
class Layout_31254 : public Layout_5d {
public:

  Layout_31254() { }

  Layout_31254(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d1 * _d2 * _d5 * _d4 + j1 * _d2 * _d5 * _d4 + j2 * _d5 * _d4 + j5 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d1 * _d2 * _d5 * _d4); j1 = (ai)/(_d2 * _d5 * _d4) % _d1; j2 = (ai)/(_d5 * _d4) % _d2; j5 = (ai)/(_d4) % _d5; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d1, d4, d2, d5 order,
// meaning d5 is stored with unit stride.
class Layout_31425 : public Layout_5d {
public:

  Layout_31425() { }

  Layout_31425(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d1 * _d4 * _d2 * _d5 + j1 * _d4 * _d2 * _d5 + j4 * _d2 * _d5 + j2 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d1 * _d4 * _d2 * _d5); j1 = (ai)/(_d4 * _d2 * _d5) % _d1; j4 = (ai)/(_d2 * _d5) % _d4; j2 = (ai)/(_d5) % _d2; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d1, d4, d5, d2 order,
// meaning d2 is stored with unit stride.
class Layout_31452 : public Layout_5d {
public:

  Layout_31452() { }

  Layout_31452(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d1 * _d4 * _d5 * _d2 + j1 * _d4 * _d5 * _d2 + j4 * _d5 * _d2 + j5 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d1 * _d4 * _d5 * _d2); j1 = (ai)/(_d4 * _d5 * _d2) % _d1; j4 = (ai)/(_d5 * _d2) % _d4; j5 = (ai)/(_d2) % _d5; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d1, d5, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_31524 : public Layout_5d {
public:

  Layout_31524() { }

  Layout_31524(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d1 * _d5 * _d2 * _d4 + j1 * _d5 * _d2 * _d4 + j5 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d1 * _d5 * _d2 * _d4); j1 = (ai)/(_d5 * _d2 * _d4) % _d1; j5 = (ai)/(_d2 * _d4) % _d5; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d1, d5, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_31542 : public Layout_5d {
public:

  Layout_31542() { }

  Layout_31542(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d1 * _d5 * _d4 * _d2 + j1 * _d5 * _d4 * _d2 + j5 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d1 * _d5 * _d4 * _d2); j1 = (ai)/(_d5 * _d4 * _d2) % _d1; j5 = (ai)/(_d4 * _d2) % _d5; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d2, d1, d4, d5 order,
// meaning d5 is stored with unit stride.
class Layout_32145 : public Layout_5d {
public:

  Layout_32145() { }

  Layout_32145(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d2 * _d1 * _d4 * _d5 + j2 * _d1 * _d4 * _d5 + j1 * _d4 * _d5 + j4 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d2 * _d1 * _d4 * _d5); j2 = (ai)/(_d1 * _d4 * _d5) % _d2; j1 = (ai)/(_d4 * _d5) % _d1; j4 = (ai)/(_d5) % _d4; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d2, d1, d5, d4 order,
// meaning d4 is stored with unit stride.
class Layout_32154 : public Layout_5d {
public:

  Layout_32154() { }

  Layout_32154(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d2 * _d1 * _d5 * _d4 + j2 * _d1 * _d5 * _d4 + j1 * _d5 * _d4 + j5 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d2 * _d1 * _d5 * _d4); j2 = (ai)/(_d1 * _d5 * _d4) % _d2; j1 = (ai)/(_d5 * _d4) % _d1; j5 = (ai)/(_d4) % _d5; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d2, d4, d1, d5 order,
// meaning d5 is stored with unit stride.
class Layout_32415 : public Layout_5d {
public:

  Layout_32415() { }

  Layout_32415(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d2 * _d4 * _d1 * _d5 + j2 * _d4 * _d1 * _d5 + j4 * _d1 * _d5 + j1 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d2 * _d4 * _d1 * _d5); j2 = (ai)/(_d4 * _d1 * _d5) % _d2; j4 = (ai)/(_d1 * _d5) % _d4; j1 = (ai)/(_d5) % _d1; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d2, d4, d5, d1 order,
// meaning d1 is stored with unit stride.
class Layout_32451 : public Layout_5d {
public:

  Layout_32451() { }

  Layout_32451(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d2 * _d4 * _d5 * _d1 + j2 * _d4 * _d5 * _d1 + j4 * _d5 * _d1 + j5 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d2 * _d4 * _d5 * _d1); j2 = (ai)/(_d4 * _d5 * _d1) % _d2; j4 = (ai)/(_d5 * _d1) % _d4; j5 = (ai)/(_d1) % _d5; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d2, d5, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_32514 : public Layout_5d {
public:

  Layout_32514() { }

  Layout_32514(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d2 * _d5 * _d1 * _d4 + j2 * _d5 * _d1 * _d4 + j5 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d2 * _d5 * _d1 * _d4); j2 = (ai)/(_d5 * _d1 * _d4) % _d2; j5 = (ai)/(_d1 * _d4) % _d5; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d2, d5, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_32541 : public Layout_5d {
public:

  Layout_32541() { }

  Layout_32541(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d2 * _d5 * _d4 * _d1 + j2 * _d5 * _d4 * _d1 + j5 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d2 * _d5 * _d4 * _d1); j2 = (ai)/(_d5 * _d4 * _d1) % _d2; j5 = (ai)/(_d4 * _d1) % _d5; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d4, d1, d2, d5 order,
// meaning d5 is stored with unit stride.
class Layout_34125 : public Layout_5d {
public:

  Layout_34125() { }

  Layout_34125(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d4 * _d1 * _d2 * _d5 + j4 * _d1 * _d2 * _d5 + j1 * _d2 * _d5 + j2 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d4 * _d1 * _d2 * _d5); j4 = (ai)/(_d1 * _d2 * _d5) % _d4; j1 = (ai)/(_d2 * _d5) % _d1; j2 = (ai)/(_d5) % _d2; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d4, d1, d5, d2 order,
// meaning d2 is stored with unit stride.
class Layout_34152 : public Layout_5d {
public:

  Layout_34152() { }

  Layout_34152(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d4 * _d1 * _d5 * _d2 + j4 * _d1 * _d5 * _d2 + j1 * _d5 * _d2 + j5 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d4 * _d1 * _d5 * _d2); j4 = (ai)/(_d1 * _d5 * _d2) % _d4; j1 = (ai)/(_d5 * _d2) % _d1; j5 = (ai)/(_d2) % _d5; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d4, d2, d1, d5 order,
// meaning d5 is stored with unit stride.
class Layout_34215 : public Layout_5d {
public:

  Layout_34215() { }

  Layout_34215(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d4 * _d2 * _d1 * _d5 + j4 * _d2 * _d1 * _d5 + j2 * _d1 * _d5 + j1 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d4 * _d2 * _d1 * _d5); j4 = (ai)/(_d2 * _d1 * _d5) % _d4; j2 = (ai)/(_d1 * _d5) % _d2; j1 = (ai)/(_d5) % _d1; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d4, d2, d5, d1 order,
// meaning d1 is stored with unit stride.
class Layout_34251 : public Layout_5d {
public:

  Layout_34251() { }

  Layout_34251(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d4 * _d2 * _d5 * _d1 + j4 * _d2 * _d5 * _d1 + j2 * _d5 * _d1 + j5 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d4 * _d2 * _d5 * _d1); j4 = (ai)/(_d2 * _d5 * _d1) % _d4; j2 = (ai)/(_d5 * _d1) % _d2; j5 = (ai)/(_d1) % _d5; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d4, d5, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_34512 : public Layout_5d {
public:

  Layout_34512() { }

  Layout_34512(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d4 * _d5 * _d1 * _d2 + j4 * _d5 * _d1 * _d2 + j5 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d4 * _d5 * _d1 * _d2); j4 = (ai)/(_d5 * _d1 * _d2) % _d4; j5 = (ai)/(_d1 * _d2) % _d5; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d4, d5, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_34521 : public Layout_5d {
public:

  Layout_34521() { }

  Layout_34521(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d4 * _d5 * _d2 * _d1 + j4 * _d5 * _d2 * _d1 + j5 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d4 * _d5 * _d2 * _d1); j4 = (ai)/(_d5 * _d2 * _d1) % _d4; j5 = (ai)/(_d2 * _d1) % _d5; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d5, d1, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_35124 : public Layout_5d {
public:

  Layout_35124() { }

  Layout_35124(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d5 * _d1 * _d2 * _d4 + j5 * _d1 * _d2 * _d4 + j1 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d5 * _d1 * _d2 * _d4); j5 = (ai)/(_d1 * _d2 * _d4) % _d5; j1 = (ai)/(_d2 * _d4) % _d1; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d5, d1, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_35142 : public Layout_5d {
public:

  Layout_35142() { }

  Layout_35142(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d5 * _d1 * _d4 * _d2 + j5 * _d1 * _d4 * _d2 + j1 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d5 * _d1 * _d4 * _d2); j5 = (ai)/(_d1 * _d4 * _d2) % _d5; j1 = (ai)/(_d4 * _d2) % _d1; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d5, d2, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_35214 : public Layout_5d {
public:

  Layout_35214() { }

  Layout_35214(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d5 * _d2 * _d1 * _d4 + j5 * _d2 * _d1 * _d4 + j2 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d5 * _d2 * _d1 * _d4); j5 = (ai)/(_d2 * _d1 * _d4) % _d5; j2 = (ai)/(_d1 * _d4) % _d2; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d5, d2, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_35241 : public Layout_5d {
public:

  Layout_35241() { }

  Layout_35241(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d5 * _d2 * _d4 * _d1 + j5 * _d2 * _d4 * _d1 + j2 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d5 * _d2 * _d4 * _d1); j5 = (ai)/(_d2 * _d4 * _d1) % _d5; j2 = (ai)/(_d4 * _d1) % _d2; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d5, d4, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_35412 : public Layout_5d {
public:

  Layout_35412() { }

  Layout_35412(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d5 * _d4 * _d1 * _d2 + j5 * _d4 * _d1 * _d2 + j4 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d5 * _d4 * _d1 * _d2); j5 = (ai)/(_d4 * _d1 * _d2) % _d5; j4 = (ai)/(_d1 * _d2) % _d4; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d3, d5, d4, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_35421 : public Layout_5d {
public:

  Layout_35421() { }

  Layout_35421(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j3 * _d5 * _d4 * _d2 * _d1 + j5 * _d4 * _d2 * _d1 + j4 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j3 = (ai)/(_d5 * _d4 * _d2 * _d1); j5 = (ai)/(_d4 * _d2 * _d1) % _d5; j4 = (ai)/(_d2 * _d1) % _d4; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d1, d2, d3, d5 order,
// meaning d5 is stored with unit stride.
class Layout_41235 : public Layout_5d {
public:

  Layout_41235() { }

  Layout_41235(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d1 * _d2 * _d3 * _d5 + j1 * _d2 * _d3 * _d5 + j2 * _d3 * _d5 + j3 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d1 * _d2 * _d3 * _d5); j1 = (ai)/(_d2 * _d3 * _d5) % _d1; j2 = (ai)/(_d3 * _d5) % _d2; j3 = (ai)/(_d5) % _d3; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d1, d2, d5, d3 order,
// meaning d3 is stored with unit stride.
class Layout_41253 : public Layout_5d {
public:

  Layout_41253() { }

  Layout_41253(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d1 * _d2 * _d5 * _d3 + j1 * _d2 * _d5 * _d3 + j2 * _d5 * _d3 + j5 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d1 * _d2 * _d5 * _d3); j1 = (ai)/(_d2 * _d5 * _d3) % _d1; j2 = (ai)/(_d5 * _d3) % _d2; j5 = (ai)/(_d3) % _d5; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d1, d3, d2, d5 order,
// meaning d5 is stored with unit stride.
class Layout_41325 : public Layout_5d {
public:

  Layout_41325() { }

  Layout_41325(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d1 * _d3 * _d2 * _d5 + j1 * _d3 * _d2 * _d5 + j3 * _d2 * _d5 + j2 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d1 * _d3 * _d2 * _d5); j1 = (ai)/(_d3 * _d2 * _d5) % _d1; j3 = (ai)/(_d2 * _d5) % _d3; j2 = (ai)/(_d5) % _d2; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d1, d3, d5, d2 order,
// meaning d2 is stored with unit stride.
class Layout_41352 : public Layout_5d {
public:

  Layout_41352() { }

  Layout_41352(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d1 * _d3 * _d5 * _d2 + j1 * _d3 * _d5 * _d2 + j3 * _d5 * _d2 + j5 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d1 * _d3 * _d5 * _d2); j1 = (ai)/(_d3 * _d5 * _d2) % _d1; j3 = (ai)/(_d5 * _d2) % _d3; j5 = (ai)/(_d2) % _d5; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d1, d5, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_41523 : public Layout_5d {
public:

  Layout_41523() { }

  Layout_41523(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d1 * _d5 * _d2 * _d3 + j1 * _d5 * _d2 * _d3 + j5 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d1 * _d5 * _d2 * _d3); j1 = (ai)/(_d5 * _d2 * _d3) % _d1; j5 = (ai)/(_d2 * _d3) % _d5; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d1, d5, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_41532 : public Layout_5d {
public:

  Layout_41532() { }

  Layout_41532(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d1 * _d5 * _d3 * _d2 + j1 * _d5 * _d3 * _d2 + j5 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d1 * _d5 * _d3 * _d2); j1 = (ai)/(_d5 * _d3 * _d2) % _d1; j5 = (ai)/(_d3 * _d2) % _d5; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d2, d1, d3, d5 order,
// meaning d5 is stored with unit stride.
class Layout_42135 : public Layout_5d {
public:

  Layout_42135() { }

  Layout_42135(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d2 * _d1 * _d3 * _d5 + j2 * _d1 * _d3 * _d5 + j1 * _d3 * _d5 + j3 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d2 * _d1 * _d3 * _d5); j2 = (ai)/(_d1 * _d3 * _d5) % _d2; j1 = (ai)/(_d3 * _d5) % _d1; j3 = (ai)/(_d5) % _d3; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d2, d1, d5, d3 order,
// meaning d3 is stored with unit stride.
class Layout_42153 : public Layout_5d {
public:

  Layout_42153() { }

  Layout_42153(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d2 * _d1 * _d5 * _d3 + j2 * _d1 * _d5 * _d3 + j1 * _d5 * _d3 + j5 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d2 * _d1 * _d5 * _d3); j2 = (ai)/(_d1 * _d5 * _d3) % _d2; j1 = (ai)/(_d5 * _d3) % _d1; j5 = (ai)/(_d3) % _d5; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d2, d3, d1, d5 order,
// meaning d5 is stored with unit stride.
class Layout_42315 : public Layout_5d {
public:

  Layout_42315() { }

  Layout_42315(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d2 * _d3 * _d1 * _d5 + j2 * _d3 * _d1 * _d5 + j3 * _d1 * _d5 + j1 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d2 * _d3 * _d1 * _d5); j2 = (ai)/(_d3 * _d1 * _d5) % _d2; j3 = (ai)/(_d1 * _d5) % _d3; j1 = (ai)/(_d5) % _d1; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d2, d3, d5, d1 order,
// meaning d1 is stored with unit stride.
class Layout_42351 : public Layout_5d {
public:

  Layout_42351() { }

  Layout_42351(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d2 * _d3 * _d5 * _d1 + j2 * _d3 * _d5 * _d1 + j3 * _d5 * _d1 + j5 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d2 * _d3 * _d5 * _d1); j2 = (ai)/(_d3 * _d5 * _d1) % _d2; j3 = (ai)/(_d5 * _d1) % _d3; j5 = (ai)/(_d1) % _d5; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d2, d5, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_42513 : public Layout_5d {
public:

  Layout_42513() { }

  Layout_42513(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d2 * _d5 * _d1 * _d3 + j2 * _d5 * _d1 * _d3 + j5 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d2 * _d5 * _d1 * _d3); j2 = (ai)/(_d5 * _d1 * _d3) % _d2; j5 = (ai)/(_d1 * _d3) % _d5; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d2, d5, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_42531 : public Layout_5d {
public:

  Layout_42531() { }

  Layout_42531(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d2 * _d5 * _d3 * _d1 + j2 * _d5 * _d3 * _d1 + j5 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d2 * _d5 * _d3 * _d1); j2 = (ai)/(_d5 * _d3 * _d1) % _d2; j5 = (ai)/(_d3 * _d1) % _d5; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d3, d1, d2, d5 order,
// meaning d5 is stored with unit stride.
class Layout_43125 : public Layout_5d {
public:

  Layout_43125() { }

  Layout_43125(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d3 * _d1 * _d2 * _d5 + j3 * _d1 * _d2 * _d5 + j1 * _d2 * _d5 + j2 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d3 * _d1 * _d2 * _d5); j3 = (ai)/(_d1 * _d2 * _d5) % _d3; j1 = (ai)/(_d2 * _d5) % _d1; j2 = (ai)/(_d5) % _d2; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d3, d1, d5, d2 order,
// meaning d2 is stored with unit stride.
class Layout_43152 : public Layout_5d {
public:

  Layout_43152() { }

  Layout_43152(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d3 * _d1 * _d5 * _d2 + j3 * _d1 * _d5 * _d2 + j1 * _d5 * _d2 + j5 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d3 * _d1 * _d5 * _d2); j3 = (ai)/(_d1 * _d5 * _d2) % _d3; j1 = (ai)/(_d5 * _d2) % _d1; j5 = (ai)/(_d2) % _d5; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d3, d2, d1, d5 order,
// meaning d5 is stored with unit stride.
class Layout_43215 : public Layout_5d {
public:

  Layout_43215() { }

  Layout_43215(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d3 * _d2 * _d1 * _d5 + j3 * _d2 * _d1 * _d5 + j2 * _d1 * _d5 + j1 * _d5 + j5; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d3 * _d2 * _d1 * _d5); j3 = (ai)/(_d2 * _d1 * _d5) % _d3; j2 = (ai)/(_d1 * _d5) % _d2; j1 = (ai)/(_d5) % _d1; j5 = (ai) % _d5; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d3, d2, d5, d1 order,
// meaning d1 is stored with unit stride.
class Layout_43251 : public Layout_5d {
public:

  Layout_43251() { }

  Layout_43251(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d3 * _d2 * _d5 * _d1 + j3 * _d2 * _d5 * _d1 + j2 * _d5 * _d1 + j5 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d3 * _d2 * _d5 * _d1); j3 = (ai)/(_d2 * _d5 * _d1) % _d3; j2 = (ai)/(_d5 * _d1) % _d2; j5 = (ai)/(_d1) % _d5; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d3, d5, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_43512 : public Layout_5d {
public:

  Layout_43512() { }

  Layout_43512(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d3 * _d5 * _d1 * _d2 + j3 * _d5 * _d1 * _d2 + j5 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d3 * _d5 * _d1 * _d2); j3 = (ai)/(_d5 * _d1 * _d2) % _d3; j5 = (ai)/(_d1 * _d2) % _d5; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d3, d5, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_43521 : public Layout_5d {
public:

  Layout_43521() { }

  Layout_43521(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d3 * _d5 * _d2 * _d1 + j3 * _d5 * _d2 * _d1 + j5 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d3 * _d5 * _d2 * _d1); j3 = (ai)/(_d5 * _d2 * _d1) % _d3; j5 = (ai)/(_d2 * _d1) % _d5; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d5, d1, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_45123 : public Layout_5d {
public:

  Layout_45123() { }

  Layout_45123(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d5 * _d1 * _d2 * _d3 + j5 * _d1 * _d2 * _d3 + j1 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d5 * _d1 * _d2 * _d3); j5 = (ai)/(_d1 * _d2 * _d3) % _d5; j1 = (ai)/(_d2 * _d3) % _d1; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d5, d1, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_45132 : public Layout_5d {
public:

  Layout_45132() { }

  Layout_45132(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d5 * _d1 * _d3 * _d2 + j5 * _d1 * _d3 * _d2 + j1 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d5 * _d1 * _d3 * _d2); j5 = (ai)/(_d1 * _d3 * _d2) % _d5; j1 = (ai)/(_d3 * _d2) % _d1; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d5, d2, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_45213 : public Layout_5d {
public:

  Layout_45213() { }

  Layout_45213(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d5 * _d2 * _d1 * _d3 + j5 * _d2 * _d1 * _d3 + j2 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d5 * _d2 * _d1 * _d3); j5 = (ai)/(_d2 * _d1 * _d3) % _d5; j2 = (ai)/(_d1 * _d3) % _d2; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d5, d2, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_45231 : public Layout_5d {
public:

  Layout_45231() { }

  Layout_45231(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d5 * _d2 * _d3 * _d1 + j5 * _d2 * _d3 * _d1 + j2 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d5 * _d2 * _d3 * _d1); j5 = (ai)/(_d2 * _d3 * _d1) % _d5; j2 = (ai)/(_d3 * _d1) % _d2; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d5, d3, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_45312 : public Layout_5d {
public:

  Layout_45312() { }

  Layout_45312(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d5 * _d3 * _d1 * _d2 + j5 * _d3 * _d1 * _d2 + j3 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d5 * _d3 * _d1 * _d2); j5 = (ai)/(_d3 * _d1 * _d2) % _d5; j3 = (ai)/(_d1 * _d2) % _d3; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d4, d5, d3, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_45321 : public Layout_5d {
public:

  Layout_45321() { }

  Layout_45321(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j4 * _d5 * _d3 * _d2 * _d1 + j5 * _d3 * _d2 * _d1 + j3 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j4 = (ai)/(_d5 * _d3 * _d2 * _d1); j5 = (ai)/(_d3 * _d2 * _d1) % _d5; j3 = (ai)/(_d2 * _d1) % _d3; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d1, d2, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_51234 : public Layout_5d {
public:

  Layout_51234() { }

  Layout_51234(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d1 * _d2 * _d3 * _d4 + j1 * _d2 * _d3 * _d4 + j2 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d1 * _d2 * _d3 * _d4); j1 = (ai)/(_d2 * _d3 * _d4) % _d1; j2 = (ai)/(_d3 * _d4) % _d2; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d1, d2, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_51243 : public Layout_5d {
public:

  Layout_51243() { }

  Layout_51243(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d1 * _d2 * _d4 * _d3 + j1 * _d2 * _d4 * _d3 + j2 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d1 * _d2 * _d4 * _d3); j1 = (ai)/(_d2 * _d4 * _d3) % _d1; j2 = (ai)/(_d4 * _d3) % _d2; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d1, d3, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_51324 : public Layout_5d {
public:

  Layout_51324() { }

  Layout_51324(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d1 * _d3 * _d2 * _d4 + j1 * _d3 * _d2 * _d4 + j3 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d1 * _d3 * _d2 * _d4); j1 = (ai)/(_d3 * _d2 * _d4) % _d1; j3 = (ai)/(_d2 * _d4) % _d3; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d1, d3, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_51342 : public Layout_5d {
public:

  Layout_51342() { }

  Layout_51342(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d1 * _d3 * _d4 * _d2 + j1 * _d3 * _d4 * _d2 + j3 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d1 * _d3 * _d4 * _d2); j1 = (ai)/(_d3 * _d4 * _d2) % _d1; j3 = (ai)/(_d4 * _d2) % _d3; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d1, d4, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_51423 : public Layout_5d {
public:

  Layout_51423() { }

  Layout_51423(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d1 * _d4 * _d2 * _d3 + j1 * _d4 * _d2 * _d3 + j4 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d1 * _d4 * _d2 * _d3); j1 = (ai)/(_d4 * _d2 * _d3) % _d1; j4 = (ai)/(_d2 * _d3) % _d4; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d1, d4, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_51432 : public Layout_5d {
public:

  Layout_51432() { }

  Layout_51432(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d1 * _d4 * _d3 * _d2 + j1 * _d4 * _d3 * _d2 + j4 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d1 * _d4 * _d3 * _d2); j1 = (ai)/(_d4 * _d3 * _d2) % _d1; j4 = (ai)/(_d3 * _d2) % _d4; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d2, d1, d3, d4 order,
// meaning d4 is stored with unit stride.
class Layout_52134 : public Layout_5d {
public:

  Layout_52134() { }

  Layout_52134(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d2 * _d1 * _d3 * _d4 + j2 * _d1 * _d3 * _d4 + j1 * _d3 * _d4 + j3 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d2 * _d1 * _d3 * _d4); j2 = (ai)/(_d1 * _d3 * _d4) % _d2; j1 = (ai)/(_d3 * _d4) % _d1; j3 = (ai)/(_d4) % _d3; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d2, d1, d4, d3 order,
// meaning d3 is stored with unit stride.
class Layout_52143 : public Layout_5d {
public:

  Layout_52143() { }

  Layout_52143(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d2 * _d1 * _d4 * _d3 + j2 * _d1 * _d4 * _d3 + j1 * _d4 * _d3 + j4 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d2 * _d1 * _d4 * _d3); j2 = (ai)/(_d1 * _d4 * _d3) % _d2; j1 = (ai)/(_d4 * _d3) % _d1; j4 = (ai)/(_d3) % _d4; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d2, d3, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_52314 : public Layout_5d {
public:

  Layout_52314() { }

  Layout_52314(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d2 * _d3 * _d1 * _d4 + j2 * _d3 * _d1 * _d4 + j3 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d2 * _d3 * _d1 * _d4); j2 = (ai)/(_d3 * _d1 * _d4) % _d2; j3 = (ai)/(_d1 * _d4) % _d3; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d2, d3, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_52341 : public Layout_5d {
public:

  Layout_52341() { }

  Layout_52341(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d2 * _d3 * _d4 * _d1 + j2 * _d3 * _d4 * _d1 + j3 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d2 * _d3 * _d4 * _d1); j2 = (ai)/(_d3 * _d4 * _d1) % _d2; j3 = (ai)/(_d4 * _d1) % _d3; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d2, d4, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_52413 : public Layout_5d {
public:

  Layout_52413() { }

  Layout_52413(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d2 * _d4 * _d1 * _d3 + j2 * _d4 * _d1 * _d3 + j4 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d2 * _d4 * _d1 * _d3); j2 = (ai)/(_d4 * _d1 * _d3) % _d2; j4 = (ai)/(_d1 * _d3) % _d4; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d2, d4, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_52431 : public Layout_5d {
public:

  Layout_52431() { }

  Layout_52431(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d2 * _d4 * _d3 * _d1 + j2 * _d4 * _d3 * _d1 + j4 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d2 * _d4 * _d3 * _d1); j2 = (ai)/(_d4 * _d3 * _d1) % _d2; j4 = (ai)/(_d3 * _d1) % _d4; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d3, d1, d2, d4 order,
// meaning d4 is stored with unit stride.
class Layout_53124 : public Layout_5d {
public:

  Layout_53124() { }

  Layout_53124(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d3 * _d1 * _d2 * _d4 + j3 * _d1 * _d2 * _d4 + j1 * _d2 * _d4 + j2 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d3 * _d1 * _d2 * _d4); j3 = (ai)/(_d1 * _d2 * _d4) % _d3; j1 = (ai)/(_d2 * _d4) % _d1; j2 = (ai)/(_d4) % _d2; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d3, d1, d4, d2 order,
// meaning d2 is stored with unit stride.
class Layout_53142 : public Layout_5d {
public:

  Layout_53142() { }

  Layout_53142(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d3 * _d1 * _d4 * _d2 + j3 * _d1 * _d4 * _d2 + j1 * _d4 * _d2 + j4 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d3 * _d1 * _d4 * _d2); j3 = (ai)/(_d1 * _d4 * _d2) % _d3; j1 = (ai)/(_d4 * _d2) % _d1; j4 = (ai)/(_d2) % _d4; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d3, d2, d1, d4 order,
// meaning d4 is stored with unit stride.
class Layout_53214 : public Layout_5d {
public:

  Layout_53214() { }

  Layout_53214(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d3 * _d2 * _d1 * _d4 + j3 * _d2 * _d1 * _d4 + j2 * _d1 * _d4 + j1 * _d4 + j4; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d3 * _d2 * _d1 * _d4); j3 = (ai)/(_d2 * _d1 * _d4) % _d3; j2 = (ai)/(_d1 * _d4) % _d2; j1 = (ai)/(_d4) % _d1; j4 = (ai) % _d4; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d3, d2, d4, d1 order,
// meaning d1 is stored with unit stride.
class Layout_53241 : public Layout_5d {
public:

  Layout_53241() { }

  Layout_53241(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d3 * _d2 * _d4 * _d1 + j3 * _d2 * _d4 * _d1 + j2 * _d4 * _d1 + j4 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d3 * _d2 * _d4 * _d1); j3 = (ai)/(_d2 * _d4 * _d1) % _d3; j2 = (ai)/(_d4 * _d1) % _d2; j4 = (ai)/(_d1) % _d4; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d3, d4, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_53412 : public Layout_5d {
public:

  Layout_53412() { }

  Layout_53412(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d3 * _d4 * _d1 * _d2 + j3 * _d4 * _d1 * _d2 + j4 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d3 * _d4 * _d1 * _d2); j3 = (ai)/(_d4 * _d1 * _d2) % _d3; j4 = (ai)/(_d1 * _d2) % _d4; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d3, d4, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_53421 : public Layout_5d {
public:

  Layout_53421() { }

  Layout_53421(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d3 * _d4 * _d2 * _d1 + j3 * _d4 * _d2 * _d1 + j4 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d3 * _d4 * _d2 * _d1); j3 = (ai)/(_d4 * _d2 * _d1) % _d3; j4 = (ai)/(_d2 * _d1) % _d4; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d4, d1, d2, d3 order,
// meaning d3 is stored with unit stride.
class Layout_54123 : public Layout_5d {
public:

  Layout_54123() { }

  Layout_54123(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d4 * _d1 * _d2 * _d3 + j4 * _d1 * _d2 * _d3 + j1 * _d2 * _d3 + j2 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d4 * _d1 * _d2 * _d3); j4 = (ai)/(_d1 * _d2 * _d3) % _d4; j1 = (ai)/(_d2 * _d3) % _d1; j2 = (ai)/(_d3) % _d2; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d4, d1, d3, d2 order,
// meaning d2 is stored with unit stride.
class Layout_54132 : public Layout_5d {
public:

  Layout_54132() { }

  Layout_54132(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d4 * _d1 * _d3 * _d2 + j4 * _d1 * _d3 * _d2 + j1 * _d3 * _d2 + j3 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d4 * _d1 * _d3 * _d2); j4 = (ai)/(_d1 * _d3 * _d2) % _d4; j1 = (ai)/(_d3 * _d2) % _d1; j3 = (ai)/(_d2) % _d3; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d4, d2, d1, d3 order,
// meaning d3 is stored with unit stride.
class Layout_54213 : public Layout_5d {
public:

  Layout_54213() { }

  Layout_54213(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d4 * _d2 * _d1 * _d3 + j4 * _d2 * _d1 * _d3 + j2 * _d1 * _d3 + j1 * _d3 + j3; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d4 * _d2 * _d1 * _d3); j4 = (ai)/(_d2 * _d1 * _d3) % _d4; j2 = (ai)/(_d1 * _d3) % _d2; j1 = (ai)/(_d3) % _d1; j3 = (ai) % _d3; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d4, d2, d3, d1 order,
// meaning d1 is stored with unit stride.
class Layout_54231 : public Layout_5d {
public:

  Layout_54231() { }

  Layout_54231(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d4 * _d2 * _d3 * _d1 + j4 * _d2 * _d3 * _d1 + j2 * _d3 * _d1 + j3 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d4 * _d2 * _d3 * _d1); j4 = (ai)/(_d2 * _d3 * _d1) % _d4; j2 = (ai)/(_d3 * _d1) % _d2; j3 = (ai)/(_d1) % _d3; j1 = (ai) % _d1; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d4, d3, d1, d2 order,
// meaning d2 is stored with unit stride.
class Layout_54312 : public Layout_5d {
public:

  Layout_54312() { }

  Layout_54312(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d4 * _d3 * _d1 * _d2 + j4 * _d3 * _d1 * _d2 + j3 * _d1 * _d2 + j1 * _d2 + j2; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d4 * _d3 * _d1 * _d2); j4 = (ai)/(_d3 * _d1 * _d2) % _d4; j3 = (ai)/(_d1 * _d2) % _d3; j1 = (ai)/(_d2) % _d1; j2 = (ai) % _d2; }
};

// 5-D <-> 1-D layout class with dimensions in d5, d4, d3, d2, d1 order,
// meaning d1 is stored with unit stride.
class Layout_54321 : public Layout_5d {
public:

  Layout_54321() { }

  Layout_54321(idx_t d1, idx_t d2, idx_t d3, idx_t d4, idx_t d5) : Layout_5d(d1, d2, d3, d4, d5) { }

  // Return 1-D offset from 5-D 'j' indices.
  virtual idx_t layout(idx_t j1, idx_t j2, idx_t j3, idx_t j4, idx_t j5) const final
    { return j5 * _d4 * _d3 * _d2 * _d1 + j4 * _d3 * _d2 * _d1 + j3 * _d2 * _d1 + j2 * _d1 + j1; }

  // set 5 'j' indices based on 1-D 'ai' input.
  virtual void unlayout(idx_t ai, idx_t& j1, idx_t& j2, idx_t& j3, idx_t& j4, idx_t& j5) const final
    { j5 = (ai)/(_d4 * _d3 * _d2 * _d1); j4 = (ai)/(_d3 * _d2 * _d1) % _d4; j3 = (ai)/(_d2 * _d1) % _d3; j2 = (ai)/(_d1) % _d2; j1 = (ai) % _d1; }
};
