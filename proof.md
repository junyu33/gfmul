$$b_i \in \{0, 1\},\forall M(x) = \sum_{i=0}^k b_ix^i \in \mathbb{F_{2^k}} \ \text{s.t.} \deg_2(M(x)) \leq \frac{k}{2}$$

$$ (P(x) + Q(x) \cdot x^k) \mod M(x) = P(x) + Q(x) + \sum_{i \in (0, k), b_i=1} (Q(x)\cdot x^i \mod x^k) + (\sum_{i \in (0, k), b_i=1} \lfloor\frac{Q(x)}{x^{k-i}}\rfloor)(\sum_{i \ne k, b_i=1} x^i)$$
