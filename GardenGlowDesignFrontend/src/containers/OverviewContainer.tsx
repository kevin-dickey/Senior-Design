// OverviewContainer.tsx
import React from 'react';
import FoldersOverview from '../components/pages/FoldersOverview';

const fakeData = [
  {
    name: 'Christmas',
    files: [
      { name: 'Home Alone' },
      { name: 'The Grinch' },
      { name: 'Elf' },
      { name: 'A Christmas Story' },
      { name: 'The Polar Express' },
    ],
  },
  {
    name: 'Halloween',
    files: [
      { name: 'Hocus Pocus' },
      { name: 'Halloween' },
      { name: 'The Nightmare Before Christmas' },
      { name: 'Beetlejuice' },
    ],
  },
  {
    name: 'Thanksgiving',
    files: [
      { name: 'Planes, Trains, and Automobiles' },
      { name: 'A Charlie Brown Thanksgiving' },
      { name: 'Friends: The One with All the Thanksgivings' },
      { name: 'The Turkey Bowl' },
    ],
  },
  {
    name: 'New Year\'s Eve',
    files: [
      { name: 'New Year’s Eve' },
      { name: 'When Harry Met Sally' },
      { name: 'Sleepless in Seattle' },
    ],
  },
  {
    name: 'Valentine\'s Day',
    files: [
      { name: 'Valentine\'s Day' },
      { name: 'The Notebook' },
      { name: '10 Things I Hate About You' },
      { name: 'La La Land' },
      { name: 'Pride and Prejudice' },
    ],
  },
  {
    name: 'Independence Day',
    files: [
      { name: 'Independence Day' },
      { name: 'Born on the Fourth of July' },
      { name: 'National Treasure' },
    ],
  },
  {
    name: 'Easter',
    files: [
      { name: 'Hop' },
      { name: 'The Ten Commandments' },
      { name: 'Peter Rabbit' },
      { name: 'Rise of the Guardians' },
    ],
  },
  {
    name: 'St. Patrick\'s Day',
    files: [
      { name: 'Leprechaun' },
      { name: 'The Luck of the Irish' },
      { name: 'Darby O\'Gill and the Little People' },
    ],
  },
  {
    name: 'Hanukkah',
    files: [
      { name: 'Eight Crazy Nights' },
      { name: 'The Hebrew Hammer' },
      { name: 'Full-Court Miracle' },
      { name: 'An American Tail' },
    ],
  },
  {
    name: 'Mother\'s Day',
    files: [
      { name: 'Mother\'s Day' },
      { name: 'Stepmom' },
      { name: 'Mamma Mia!' },
      { name: 'Terms of Endearment' },
    ],
  },
  {
    name: 'Father\'s Day',
    files: [
      { name: 'Father of the Bride' },
      { name: 'The Pursuit of Happyness' },
      { name: 'Big Fish' },
      { name: 'The Lion King' },
    ],
  },
  {
    name: 'Veterans Day',
    files: [
      { name: 'Saving Private Ryan' },
      { name: 'Hacksaw Ridge' },
      { name: 'Fury' },
      { name: 'Flags of Our Fathers' },
      { name: 'American Sniper' },
    ],
  },
  {
    name: 'Memorial Day',
    files: [
      { name: 'Top Gun' },
      { name: 'Glory' },
      { name: 'We Were Soldiers' },
    ],
  },
  {
    name: 'Labor Day',
    files: [
      { name: 'Labor Day' },
      { name: 'Working Girl' },
      { name: '9 to 5' },
      { name: 'The Devil Wears Prada' },
    ],
  },
  {
    name: 'April Fool\'s Day',
    files: [
      { name: 'April Fool\'s Day' },
      { name: 'Jackass: The Movie' },
      { name: 'The Truman Show' },
      { name: 'Fool\'s Gold' },
    ],
  },
  {
    name: 'Fast and Furious',
  files: [
    { name: 'Fast and Furious 1' },
    { name: 'Fast and Furious 2' },
    { name: 'Fast and Furious 3' },
    { name: 'Fast and Furious 4' },
    { name: 'Fast and Furious 5' },
    { name: 'Fast and Furious 6' },
    { name: 'Fast and Furious 7' },
    { name: 'Fast and Furious 8' },
    { name: 'Fast and Furious 9' },
    { name: 'Fast and Furious 10' },
    { name: 'Fast and Furious 11' },
    { name: 'Fast and Furious 12' },
    { name: 'Fast and Furious 13' },
    { name: 'Fast and Furious 14' },
    { name: 'Fast and Furious 15' },
  ],
  }
];


const FoldersOverviewContainer: React.FC = () => {
  return <FoldersOverview folders={fakeData} />;
};

export default FoldersOverviewContainer;
