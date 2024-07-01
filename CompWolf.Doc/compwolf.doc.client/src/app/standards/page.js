import { CodeViewer, SimpleReference } from "@/lib/CodeComponents";

export default function Standards() {
    return (
        <div>
            <h1>Library Standards</h1>
            <section>This page denotes the standards used by the CompWolf C++ libraries.
                Funamentally, CompWolf must be:
                <ol>
                    <li>Correct</li>
                    <li>Easy to use</li>
                    <li>Quick to use</li>
                    <li>Be usable with third party tools</li>
                    <li>Performant</li>
                </ol>
            </section>
            <section>Code must be readable and correct, including:
                <ul>
                    <li>Names must describe the entity, and not use abbreviations outside of commonly known abbreviations like GPU.</li>
                    <li>Function parameters must be const correct.</li>
                </ul>
            </section>
            <h2>Naming Conventions</h2>
            <section>CompWolf uses the same naming conventions as the C++ standard library, that is:
                <ul>
                    <li>Except template parameters, all names are written as snake_case (all lowercase with the words in a name being separated by underscores).</li>
                    <li>Template parameters are written as PascalCase (first letter of each word is uppercase; other letters are lowercase.).</li>
                    <li>For example:
                        <CodeViewer>
{`float score_multiplier = 1.5f;
template <typename ContainedType, int PixelWidth, int PixelHeight> class text_box;
void apply_poison(float damage_per_second, float duration);`}
                        </CodeViewer>
                    </li>
                </ul>
            </section>
            <section>
                Getter-methods should be named "[variable name]", while setter-methods should be named "set_[variable name]".
            </section>
            <section>Unless a value is an aggregate type, data members should be private.
                The member may (if appropriate) simply be accessed by a getter-method returning a reference to the member.
            </section>
            <section>All entities must have a comment above them describing the entity.
                <ul>
                    <li>The comment must be written in the JavaDocs style:
                        <ul>
                            <li>The comment must be a multi-line comment, as in: /* comment */.</li>
                            <li>The comment must start by describing the entity in general, followed by extra, more specific information.</li>
                            <li>The following types of specific info must have the following prefixes.
                                These types of info should be ordered as in the table; this info should be the last info in the comment.
                                <ol>
                                    <li>A Template Parameter: "@typeparam [name of parameter] "</li>
                                    <li>A Parameter: "@param [name of parameter] "</li>
                                    <li>The function's return values: "@return "</li>
                                    <li>An exception that may be thrown by the function: "@throws [type of exception] "</li>
                                    <li>Related entities: "@see "</li>
                                </ol>
                            </li>
                        </ul>
                    </li>
                </ul>
            </section>
            <section>Use 4 spaces as indention (not tabs). No lines may be more than 80 characters wide.</section>
        </div>
    );
}
