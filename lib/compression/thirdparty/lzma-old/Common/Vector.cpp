// Common/Vector.cpp

#include "StdAfx.h"

#include <string.h>

#include "Vector.h"

CBaseRecordVector::~CBaseRecordVector()
  { delete []((unsigned char *)_items); }
void CBaseRecordVector::Clear() 
  { DeleteFrom(0); }
void CBaseRecordVector::DeleteBack() 
  { Delete(_size - 1); }
void CBaseRecordVector::DeleteFrom(int index)
  { Delete(index, _size - index); }

void CBaseRecordVector::ReserveOnePosition()
{
  if(_size != _capacity)
    return;
  int delta;
  if (_capacity > 64)
    delta = _capacity / 2;
  else if (_capacity > 8)
    delta = 8;
  else
    delta = 4;
  Reserve(_capacity + delta);
}

void CBaseRecordVector::Reserve(int newCapacity)
{
  if(newCapacity <= _capacity)
    return;
  unsigned char *p = new unsigned char[newCapacity * _itemSize];
  int numRecordsToMove = _capacity;
  memmove(p, _items, _itemSize * numRecordsToMove);
  delete [](unsigned char *)_items;
  _items = p;
  _capacity = newCapacity;
}

void CBaseRecordVector::MoveItems(int destIndex, int srcIndex)
{
  memmove(((unsigned char *)_items) + destIndex * _itemSize, 
    ((unsigned char  *)_items) + srcIndex * _itemSize, 
    _itemSize * (_size - srcIndex));
}

void CBaseRecordVector::InsertOneItem(int index)
{
  ReserveOnePosition();
  MoveItems(index + 1, index);
  _size++;
}

void CBaseRecordVector::Delete(int index, int num)
{
  TestIndexAndCorrectNum(index, num);
  if (num > 0)
  {
    MoveItems(index, index + num);
    _size -= num;
  }
}

