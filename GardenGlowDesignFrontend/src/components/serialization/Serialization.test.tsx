import {Show} from './Show';
import {PixelGrid} from './Layout';
import {Effect} from "./Effect";

test('serializes simple 10x10 layout', () => {
    const grid = new PixelGrid(10, 10);
    const json = JSON.stringify(grid);
    expect(json).toEqual('{"width":10,"height":10}');
});


test('serializes basic rainbow effect', () => {
    const effect = new Effect('rainbow', 1000, 5000, 0);
    const json = JSON.stringify(effect);
    expect(json).toEqual('{"name":"rainbow","startTime":1000,"duration":5000,"layer":0}');
});

test('serializes show file containing rainbow', () => {
    const effect = new Effect('rainbow', 1000, 5000, 0);
    const grid = new PixelGrid(10, 10);
    const show = new Show('test', 10000);
    show.addEffect(effect);
    show.addLayout(grid);
    const json = JSON.stringify(show);

    expect(json).toEqual(`{\
"name":"test",\
"duration":10000,\
"effects":[\
{"name":"rainbow","startTime":1000,"duration":5000,"layer":0}\
],\
"layouts":[\
{"width":10,"height":10}\
]\
}`);
});