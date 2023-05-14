## 버튼
 - [ ] 버튼 위에 마우스 올릴시 하이라이팅

- - -

충돌판정:
```cpp
if(radius >= other->radius) {
    if(dist_between_center <= radius + other->radius/3) {
        return true;
    }
}
else {
    if(dist_between_center <= radius/3 + other->radius) {
        return true;
    }
}
```
증가하는 반지름:
```cpp
sqrt(pow(player.getRadius(), 2) + pow((*iter)->getRadius(), 6)) - player.getRadius()
```
증가함수:
```cpp
radius = -(target_radius-prev_radius)*pow(trans_count++/30.0-1, 2) + target_radius;
```
