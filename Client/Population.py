import random
import math
from threading import Lock

class EA:
    MUTATION = 0.01
    POPULATION_GROWTH_FACTOR = 0.005
    FOCUS_WEIGHT = 1/10000

    PRED_v_PREY = 0
    LONE_v_HERD = 1
    STR_v_MOB = 2
    REP_v_LIF = 3
    SENSES = 4
    AGE = 5

    def __init__(self, init_size):
        self.population = []
        self.evaluation = []
        self.focus = [0,0,0,0,0]
        self.stats = [0,0,0,0,0]
        self.stat_lock = Lock()
        self.pop_lock = Lock()
        self.init_population(init_size)

    def init_population(self, size):
        for i in range(size):
            self.population.append([])
            for j in range(4):
                self.population[i].append(random.gauss(0, 1))
            self.population[i].append(math.fabs(random.gauss(0,1))) #Senses
            self.population[i].append(random.randint(0,2)) #Current age
        self.calculate_stats()

    def calculate_stats(self):
        for i in range(5):
            self.stats[i] = sum(p[i] for p in self.population) / float(len(self.population))

    def next_generation(self):
        pop_size = len(self.population)
        self.age()
        self.evaluate()
        d_pop = pop_size - len(self.population)
        #child_count = int(d_pop + self.POPULATION_GROWTH_FACTOR * pop_size * (1 + random.gauss(0, (self.stats[self.REP_v_LIF] + 5) / 10)))
        self.create_children(self.select(d_pop*2))
        self.calculate_stats()

    def age(self):
        self.population = [i for i in self.population if i[self.AGE] <= (1 + self.stats[self.REP_v_LIF] / 5.0) * 2]
        for i in range(len(self.population)):
            self.population[i][5] += 1

    def create_children(self, indices):
        for i in range(len(indices) // 2):
            child = self.combine(self.population[indices[2*i]], self.population[indices[2*i+1]])
            if random.uniform(0,1) < self.MUTATION:
                child = self.mutate(child)
            self.population.append(child)

    def mutate(self, child):
        i = random.randint(0, 4)
        child[i] = max(min(child[i] + random.gauss(0, 1), 5), -5)
        return child

    def select(self, count):
        pop_count = len(self.population)
        rep_count = 2 * pop_count // 3
        if self.evaluation == None:
            return [i % pop_count for i in random.sample(range(count), count)]
        else:
            indices = []
            unused_len = 0
            i = 0
            while i < count:
                i += 1
                if unused_len == 0:
                    unused = random.sample(range(pop_count), rep_count)
                    unused_len = rep_count
                    sm = sum([self.evaluation[j] for j in unused])
                rand = random.uniform(0, sm)
                index = 0
                curr = self.evaluation[unused[index]]
                while curr < rand and index < unused_len-1:
                    index += 1
                    curr += self.evaluation[unused[index]]
                sm -= self.evaluation[unused[index]]
                indices.append(unused.pop(index))
                unused_len -= 1
            return indices

    def combine(self, father, mother):
        child = [0,0,0,0,0,0]
        for i in range(5):
            mx = max(father[i], mother[i])
            mn = min(father[i], mother[i])
            if self.focus[i] < 0:
                child[i] = max(mn + (mn-mx) * (-self.focus[i] * self.FOCUS_WEIGHT), -5)
            elif self.focus[i] > 0:
                child[i] = min(mx + (mx-mn) * (self.focus[i] * self.FOCUS_WEIGHT), 5)
            else:
                child[i] = (mx + mn) / 2
        return child

    def evaluate(self):
        self.evaluation = [i[0] * self.focus[0] +
                           i[1] * self.focus[1] +
                           i[2] * self.focus[2] +
                           i[3] * self.focus[3] +
                           i[4] * self.focus[4]
                          for i in self.population]
        mx = max(self.evaluation)
        if mx == 0:
            self.evaluation = None
        else:
            self.evaluation = [i / mx for i in self.evaluation]

if __name__ == '__main__':
    import numpy as np
    import time
    import matplotlib.pyplot as plot

    ea = EA(10000)
    ea.focus = [1, 2, 3, 4, 5]
    stats = []
    timings = []
    for i in range(100):
        start = time.time()
        ea.next_generation()
        end = time.time()
        print i
        stats.append(ea.stats[:])
        timings.append(end - start)

    print "Original Stats"
    print stats[0]
    print "Final Stats"
    print ea.stats
    print "Average iteration time:", sum(timings) / len(timings)

    x = range(len(stats))
    plot.ylim(-5, 5)
    pvp, = plot.plot(x, [l[0] for l in stats], label='Pred vs Prey')
    lvh, = plot.plot(x, [l[1] for l in stats], label='Lone vs Herd')
    svs, = plot.plot(x, [l[2] for l in stats], label='Strength vs Speed')
    rvl, = plot.plot(x, [l[3] for l in stats], label='Reproduction vs Lifespan')
    s, = plot.plot(x, [l[4] for l in stats], label='Senses')
    plot.figlegend([pvp, lvh, svs, rvl, s], ['Pred vs Prey', 'Lone vs Herd', 'Strength vs Speed', 'Reproduction vs Lifespan', 'Senses'], 'upper center')
    plot.show()
