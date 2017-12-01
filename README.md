<h1 align=center>Skill Than Luck</h1>
<p align=center>This is a simple game.</p>

<br>

>If you have any questions, please contact "ldayou@me.com".

<br><br>
## Goal
주사위의 확률을 이용한 게임으로 여러 가지 방해요소를 피하고 여러가지 아이템을 사용해 지정된 도착 위치로 무사히 도착하면된다.

It is a game using probability of dice. You have to avoid various obstacles and arrive at the destination safely using various items.

<br><br>

## How to play
<ul>
    <li>주사위를 굴린다.(단, 주사위를 굴릴 수 있는 횟수가 정해져 있고, 모두 소진하면 게임이 종료된다.)<br>
    Roll the dice.(However, the number of times the dice you can be rolled is fixed, and when all is exhausted, the game is end.)
    <li>주사위의 눈금 수 만큼 이동하거나 공격 할 수 있다.<br>
    You can move or attack by the nuber of the dice scale.
    <li>죽일 수 있는 NPC와 파괴할 수 있는 벽이 있다.<br>
    There are NPCs that you can be killed and walls that can be destroyed.
        <ul>
            <li>NPC와 벽은 공격해야 하는 단계가 정해져 있다.
            <li>공격하면 단계가 한 단계씩 내려간다.
        </ul>
    </li>
    <li>출발지점에서부터 도착지점까지 정해진 횟수 안에 도착해야 해당 라운드를 성공하고, 모든 라운드를 성공하면 게임이 종료된다.<br>
    You must arrive within a certain number of times from the starting point to the arrival point to succeed the round, and if all rounds are successfully finished, the game is ends.
    <li>기타 Help 참조
</ul>

<br><br>
## Help
```html
    당신은 ▲입니다. NPC를 피해 도착지점까지 무사히 도착하세요!
    You are ▲. Arrive safely to your destination avoiding NPC!
    
    ● Keys                                   ● 게임 요소(Game elements)

        * ↑ : 위로 이동(UP)                      * □ : 1단계 벽(Level 1 wall)

        * ↓ : 아래로 이동(DOWN)                   * ■ : 2단계 벽(Level 2 wall)

        * ← : 왼쪽으로 이동(LEFT)                 * ▣ : 3단계 벽(Level 3 wall)

        * → : 오른쪽으로 이동(RIGHT)               * ♤ : 1단계 NPC(Level 1 NPC)

        * c : 갈고리 발사(Hook launch)            * ♠ : 2단계 NPC(Level 2 NPC)

        * Space bar : 미사일 발사(Missile launch)       * ＊ : 아이템 사용(Use Item)

        * Q : 초기화면에서 도움말(Help on the Home screen)        * ＝,∥ : 미사일(Missile)

    ● Items

          아이템 주사위의 눈금이 짝수면 유리한 아이템, 홀수면 불리한 아이템 중 랜덤으로 실행됩니다.
          An advantageous item appears if the scale of the dice is even.
          An disadvantages item appears if the scale of the dice is odd.

          ● 유리한 아이템(Advantageous item)                             ● 불리한 아이템

            * NPC 일시 정지(NPC pause)                                    * NPC 속도 증가(NPC speed increase)

            * 조작 횟수 +2(Number of operations + 2)                      * 조작 횟수 반감(Half the number of operations)

            * 모든 부실 수 있는 벽 단계 감소(Reduces all wall level)       * 플레이어 시야 가리기(Hide player view)

            * 탱크 일시 무적(Tank Temporary Invincibility)                * 모든 부실 수 있는 벽 단계 증가(All wall level increases)

            * 원하는 NPC 하나 삭제(Delete the desired NPC)                 * NPC 추가(Add NPC)
    
```

<br><br>
## Play Screen
<img src=https://github.com/pooi/SkillThanLuck/blob/master/image/001.PNG><br>
<img src=https://github.com/pooi/SkillThanLuck/blob/master/image/002.PNG><br>
<img src=https://github.com/pooi/SkillThanLuck/blob/master/image/003.PNG><br>

<br><br>
## How to give scores
각 라운드별 획득 점수의 총합(Total number of points earned per round)
<ul>
    <li>라운드별 점수 채점 방식은 다음의 항목들로 인해 결정된다.<br>
    Scoring by round is determined by the following items.
        <ul>
            <li>각각의 파괴한 객체(NPC, 벽) X 100 (Each destroyed object (NPC, wall) X 100)
            <li>남은 조작횟수 X 200 (Remaining operation count X 200)
        </ul>
    </li>
</ul>

<br><br>
## License

    Copyright 2016 Taewoo You

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
