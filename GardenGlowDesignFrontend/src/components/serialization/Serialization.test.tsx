import {Show} from './Show';
import {GridLayout} from './Layout';
import {Effect} from "./Effect";
import {Pair} from "./Pair";

test('serializes simple 10x10 layout', () => {
    const grid = new GridLayout(10, 10);
    const json = JSON.stringify(grid);
    expect(json).toEqual('{"shape":"grid","width":10,"height":10}');
});

const effect = new Effect('rainbow', new Pair(0, 0), new Pair(20, 20), 1000, 5000, undefined);

test('serializes basic rainbow effect', () => {
    const json = JSON.stringify(effect);
    expect(json).toEqual('{"name":"rainbow","startTime":1000,"duration":5000}');
});

test('serializes show file containing rainbow', () => {
    const grid = new GridLayout(10, 10);
    const show = new Show('test', 10000);
    show.addEffect(effect);
    show.addLayout(grid);
    const json = JSON.stringify(show);

    expect(json).toEqual(`{\
"name":"test",\
"duration":10000,\
"effects":[\
{"name":"rainbow","startTime":1000,"duration":5000}\
],\
"layouts":[\
{"shape":"grid","width":10,"height":10}\
]\
}`);
});