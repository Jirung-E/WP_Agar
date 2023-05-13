#include "Virus.h"


Virus::Virus(const Point& position) : color { White } {
    cells.push_back(new Cell { position });
}


void Virus::setUp() {
    for(auto e : cells) {
        e->setUp();
    }
}

void Virus::draw(const HDC& hdc, const Map& map, const RECT& valid_area) const {
    for(auto e : cells) {
        e->draw(hdc, map, valid_area);
    }
}


void Virus::move(const Vector& vector, const Map& map) {
    for(auto e : cells) {
        e->move(vector, map);
    }
}

void Virus::move(const Map& map) {
    for(auto e : cells) {
        e->move(map);
    }
}

//bool Virus::collideWith(const Cell* other) {
//    for(auto e : cells) {
//        if(e->collideWith(other)) {
//            return true;
//        }
//    }
//    return false;
//}

//void Virus::eat(Cell* cell) {
//    for(auto e : cells) {
//        
//    }
//}

void Virus::growUp() {
    for(auto e : cells) {
        e->growUp();
    }
}

Point Virus::getCenterPoint() const {
    // ��ġ���� ���?
    // �ƹ��͵� ������..?
    // �ƹ��͵� ���� �ʵ��� �ؾ���.
    // ���� ���� ������ ���� ���� 1���̰�, �׳��� �����ٰ� ������ �ɶ�.
    // ���� �ѳ��� ������ �������� ����.
    // �ٵ�, �̰� �÷��̾� �����̰�,
    // ���� ������ �׳� ������.
    Point center { 0, 0 };
    for(auto e : cells) {
        center.x += e->position.x;
        center.y += e->position.y;
    }
    center.x /= cells.size();
    center.y /= cells.size();
    return center;
}

double Virus::getRadius() const {
    Point p = getCenterPoint();
    double max = 0;
    for(auto e : cells) {
        if(e->getRadius() > max) {
            max = e->getRadius();
        }
    }
    for(auto e : cells) {
        if((e->position - p).scalar() > max) {
            max = (e->position - p).scalar();
        }
    }
    return max;
}


double Virus::getSize() const {
    // ���� ���̸� ���� �ʿ�� ����.
    double result = 0;
    for(auto e : cells) {
        result += pow(e->getRadius(), 2);
    }
    return result;
}


void Virus::split() {
    int size = cells.size();
    std::list<Cell*>::iterator iter = cells.begin();
    for(int i=0; i<size; i++) {
        Cell* c = (*iter)->split();
        if(c != nullptr) {
            cells.push_back(c);
        }
        iter++;
    }
}