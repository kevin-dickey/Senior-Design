import {Show} from "../components/serialization/Show";
import {RainbowEffect} from "../components/serialization/Effect";
import {Pair} from "../components/serialization/Pair";
import {GridLayout} from "../components/serialization/Layout";

export const makeShow = () => {
    const show = new Show('Basic Show File');
    const effect = RainbowEffect.emptyEffect();

    show.addEffect(effect);
    const effect2 = new RainbowEffect(new Pair(0, 0), new Pair(16, 16),
        1000, 1000, ['#420', '#696969'], 1000, 'Crazy Train');
    show.addEffect(effect2);

    const grid = new GridLayout(10, 10);
    show.addLayout(grid);

    return show;
}
